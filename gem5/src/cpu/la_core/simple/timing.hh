/*
 * Copyright (c) 2012-2013,2015 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2002-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Steve Reinhardt
 *          Samuel Steffl
 */

#ifndef __CPU_LA_CORE_SIMPLE_TIMING_HH__
#define __CPU_LA_CORE_SIMPLE_TIMING_HH__

#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

#include "cpu/la_core/la_cache/cache.hh"
#include "cpu/la_core/la_cache/cache_tlb.hh"
#include "cpu/la_core/la_cache/translation.hh"
#include "cpu/la_core/la_exec/exec_unit.hh"
#include "cpu/la_core/la_insn.hh"
#include "cpu/la_core/packet.hh"
#include "cpu/la_core/req_state.hh"
#include "cpu/la_core/scratchpad/scratchpad.hh"
#include "cpu/la_core/simple/base.hh"
#include "cpu/la_core/simple/exec_context.hh"
#include "cpu/translation.hh"
#include "mem/request.hh"
#include "params/TimingLACoreSimpleCPU.hh"

class TimingLACoreSimpleCPU : public BaseLACoreSimpleCPU
{
  public:

    TimingLACoreSimpleCPU(TimingLACoreSimpleCPUParams * params);
    virtual ~TimingLACoreSimpleCPU();

    void init() override;

  private:

    /*
     * If an access needs to be broken into fragments, currently at most two,
     * the the following two classes are used as the sender state of the
     * packets so the CPU can keep track of everything. In the main packet
     * sender state, there's an array with a spot for each fragment. If a
     * fragment has already been accepted by the CPU, aka isn't waiting for
     * a retry, it's pointer is NULL. After each fragment has successfully
     * been processed, the "outstanding" counter is decremented. Once the
     * count is zero, the entire larger access is complete.
     */
    class SplitMainSenderState : public Packet::SenderState
    {
      public:
        int outstanding;
        PacketPtr fragments[2];

        int
        getPendingFragment()
        {
            if (fragments[0]) {
                return 0;
            } else if (fragments[1]) {
                return 1;
            } else {
                return -1;
            }
        }
    };

    class SplitFragmentSenderState : public Packet::SenderState
    {
      public:
        SplitFragmentSenderState(PacketPtr _bigPkt, int _index) :
            bigPkt(_bigPkt), index(_index)
        {}
        PacketPtr bigPkt;
        int index;

        void
        clearFromParent()
        {
            SplitMainSenderState * main_send_state =
                dynamic_cast<SplitMainSenderState *>(bigPkt->senderState);
            main_send_state->fragments[index] = NULL;
        }
    };

    class FetchTranslation : public BaseTLB::Translation
    {
      protected:
        TimingLACoreSimpleCPU *cpu;

      public:
        FetchTranslation(TimingLACoreSimpleCPU *_cpu)
            : cpu(_cpu)
        {}

        void
        markDelayed()
        {
            assert(cpu->_status == BaseLACoreSimpleCPU::Running);
            cpu->_status = ITBWaitResponse;
        }

        void
        finish(const Fault &fault, RequestPtr req, ThreadContext *tc,
               BaseTLB::Mode mode)
        {
            cpu->sendFetch(fault, req, tc);
        }
    };
    FetchTranslation fetchTranslation;

    void threadSnoop(PacketPtr pkt, ThreadID sender);
    void sendData(RequestPtr req, uint8_t *data, uint64_t *res, bool read);
    void sendSplitData(RequestPtr req1, RequestPtr req2, RequestPtr req,
                       uint8_t *data, bool read);

    void translationFault(const Fault &fault);

    PacketPtr buildPacket(RequestPtr req, bool read);
    void buildSplitPacket(PacketPtr &pkt1, PacketPtr &pkt2,
            RequestPtr req1, RequestPtr req2, RequestPtr req,
            uint8_t *data, bool read);

    bool handleReadPacket(PacketPtr pkt);
    // This function always implicitly uses dcache_pkt.
    bool handleWritePacket();

    /**
     * A TimingCPUPort overrides the default behaviour of the
     * recvTiming and recvRetry and implements events for the
     * scheduling of handling of incoming packets in the following
     * cycle.
     */
    class TimingCPUPort : public MasterPort
    {
      public:

        TimingCPUPort(const std::string& _name, TimingLACoreSimpleCPU* _cpu)
            : MasterPort(_name, _cpu), cpu(_cpu), retryRespEvent(this)
        { }

      protected:

        TimingLACoreSimpleCPU* cpu;

        struct TickEvent : public Event
        {
            PacketPtr pkt;
            TimingLACoreSimpleCPU *cpu;

            TickEvent(TimingLACoreSimpleCPU *_cpu) : pkt(NULL), cpu(_cpu) {}
            const char *description() const { return "Timing CPU tick"; }
            void schedule(PacketPtr _pkt, Tick t);
        };

        EventWrapper<MasterPort, &MasterPort::sendRetryResp> retryRespEvent;
    };

    class IcachePort : public TimingCPUPort
    {
      public:

        IcachePort(TimingLACoreSimpleCPU *_cpu)
            : TimingCPUPort(_cpu->name() + ".icache_port", _cpu),
              tickEvent(_cpu)
        { }

      protected:

        virtual bool recvTimingResp(PacketPtr pkt);

        virtual void recvReqRetry();

        struct ITickEvent : public TickEvent
        {

            ITickEvent(TimingLACoreSimpleCPU *_cpu)
                : TickEvent(_cpu) {}
            void process();
            const char *description() const { return "Timing CPU icache tick"; }
        };

        ITickEvent tickEvent;

    };

    class DcachePort : public TimingCPUPort
    {
      public:

        DcachePort(TimingLACoreSimpleCPU *_cpu)
            : TimingCPUPort(_cpu->name() + ".dcache_port", _cpu),
              tickEvent(_cpu)
        {
           cacheBlockMask = ~(cpu->cacheLineSize() - 1);
        }

        Addr cacheBlockMask;
      protected:

        /** Snoop a coherence request, we need to check if this causes
         * a wakeup event on a cpu that is monitoring an address
         */
        virtual void recvTimingSnoopReq(PacketPtr pkt);
        virtual void recvFunctionalSnoop(PacketPtr pkt);

        virtual bool recvTimingResp(PacketPtr pkt);

        virtual void recvReqRetry();

        virtual bool isSnooping() const {
            return true;
        }

        struct DTickEvent : public TickEvent
        {
            DTickEvent(TimingLACoreSimpleCPU *_cpu)
                : TickEvent(_cpu) {}
            void process();
            const char *description() const { return "Timing CPU dcache tick"; }
        };

        DTickEvent tickEvent;

    };

    void updateCycleCounts();

    IcachePort icachePort;
    DcachePort dcachePort;

    PacketPtr ifetch_pkt;
    PacketPtr dcache_pkt;

    Cycles previousCycle;

  protected:

     /** Return a reference to the data port. */
    MasterPort &getDataPort() override { return dcachePort; }

    /** Return a reference to the instruction port. */
    MasterPort &getInstPort() override { return icachePort; }

  public:

    DrainState drain() override;
    void drainResume() override;

    void switchOut() override;
    void takeOverFrom(BaseCPU *oldCPU) override;

    void verifyMemoryMode() const override;

    void activateContext(ThreadID thread_num) override;
    void suspendContext(ThreadID thread_num) override;

    Fault readMem(Addr addr, uint8_t *data, unsigned size,
                  Request::Flags flags) override;

    Fault initiateMemRead(Addr addr, unsigned size,
                          Request::Flags flags) override;

    Fault writeMem(uint8_t *data, unsigned size,
                   Addr addr, Request::Flags flags, uint64_t *res) override;

    void fetch();
    void sendFetch(const Fault &fault, RequestPtr req, ThreadContext *tc);
    void completeIfetch(PacketPtr );
    void completeDataAccess(PacketPtr pkt);
    void advanceInst(const Fault &fault);

    /** This function is used by the page table walker to determine if it could
     * translate the a pending request or if the underlying request has been
     * squashed. This always returns false for the simple timing CPU as it never
     * executes any instructions speculatively.
     * @ return Is the current instruction squashed?
     */
    bool isSquashed() const { return false; }

    /**
     * Print state of address in memory system via PrintReq (for
     * debugging).
     */
    void printAddr(Addr a);

    /**
     * Finish a DTB translation.
     * @param state The DTB translation state.
     */
    void finishTranslation(WholeTranslationState *state);

  private:

    typedef EventWrapper<TimingLACoreSimpleCPU,
        &TimingLACoreSimpleCPU::fetch> FetchEvent;
    FetchEvent fetchEvent;

    struct IprEvent : Event {
        Packet *pkt;
        TimingLACoreSimpleCPU *cpu;
        IprEvent(Packet *_pkt, TimingLACoreSimpleCPU *_cpu, Tick t);
        virtual void process();
        virtual const char *description() const;
    };

    /**
     * Check if a system is in a drained state.
     *
     * We need to drain if:
     * <ul>
     * <li>We are in the middle of a microcode sequence as some CPUs
     *     (e.g., HW accelerated CPUs) can't be started in the middle
     *     of a gem5 microcode sequence.
     *
     * <li>Stay at PC is true.
     *
     * <li>A fetch event is scheduled. Normally this would never be the
     *     case with microPC() == 0, but right after a context is
     *     activated it can happen.
     * </ul>
     */
    bool isDrained() {
        LACoreSimpleExecContext& t_info = *threadInfo[curThread];
        LACoreThread* thread = t_info.thread;

        return thread->microPC() == 0 && !t_info.stayAtPC &&
               !fetchEvent.scheduled();
    }

    /**
     * Try to complete a drain request.
     *
     * @returns true if the CPU is drained, false otherwise.
     */
    bool tryCompleteDrain();

//=========================================================================
// Extra port and functions needed for LACore extension
//=========================================================================

  public:
    class LACachePort : public MasterPort
    {
      public:
        LACachePort(const std::string& name, TimingLACoreSimpleCPU* cpu,
            uint8_t channels);
        ~LACachePort();

        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;

        bool startTranslation(Addr addr, uint8_t *data, uint64_t size, 
            BaseTLB::Mode mode, ThreadContext *tc, uint64_t req_id,
            uint8_t channel);
        bool sendTimingReadReq(Addr addr, uint64_t size, ThreadContext *tc,
            uint64_t req_id, uint8_t channel);
        bool sendTimingWriteReq(Addr addr, uint8_t *data, uint64_t size,
            ThreadContext *tc, uint64_t req_id, uint8_t channel);

        std::vector< std::deque<PacketPtr> > laCachePktQs;
        TimingLACoreSimpleCPU *cpu;
    };

    class ScratchPort : public MasterPort
    {
      public:
        ScratchPort(const std::string& name, TimingLACoreSimpleCPU* cpu,
            uint64_t channel);
        ~ScratchPort();

        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;

        bool sendTimingReadReq(Addr addr, uint64_t size, ThreadContext *tc, 
            uint64_t req_id);
        bool sendTimingWriteReq(Addr addr, uint8_t *data, uint64_t size,
            ThreadContext *tc, uint64_t req_id);

        TimingLACoreSimpleCPU *cpu;
        const uint64_t channel;
    };

    //used to identify ports uniquely to whole memory system
    MasterID laCacheMasterId;
    std::vector<MasterID> scratchMasterIds;

    LACachePort laCachePort;
    std::vector<ScratchPort> scratchPorts;

    MasterPort &getLACachePort() { return laCachePort; }

    BaseMasterPort &getMasterPort(const std::string &if_name,
                                  PortID idx = InvalidPortID) override;
    
    LACoreLACache * laCache;
    LACoreLACacheTLB * laCacheTLB;
    LACoreScratchpad * scratchpad;

//=========================================================================
// Read/Write to Mem/Scratch interface functions!
//=========================================================================

    //called by memory/scratchpad ports on data received during load
    void recvTimingResp(LACorePacketPtr pkt);

    //atomic LACore accesses
    Fault writeLAMemAtomic(Addr addr, uint8_t *data, uint64_t size,
        ThreadContext *tc) override;
    Fault writeLAScratchAtomic(Addr addr, uint8_t *data, uint64_t size,
        ThreadContext *tc) override;
    Fault readLAMemAtomic(Addr addr, uint8_t *data, uint64_t size,
        ThreadContext *tc) override;
    Fault readLAScratchAtomic(Addr addr, uint8_t *data, uint64_t size,
        ThreadContext *tc) override;

    //timing LACore accesses
    bool writeLAMemTiming(Addr addr, uint8_t *data, uint8_t size, 
        ThreadContext *tc, uint8_t channel,
        std::function<void(void)> callback) override;
    bool writeLAScratchTiming(Addr addr, uint8_t *data, uint8_t size, 
        ThreadContext *tc, uint8_t channel,
        std::function<void(void)> callback) override;
    bool readLAMemTiming(Addr addr, uint8_t size, ThreadContext *tc, 
        uint8_t channel,
        std::function<void(uint8_t*,uint8_t)> callback) override;
    bool readLAScratchTiming(Addr addr, uint8_t size, ThreadContext *tc, 
        uint8_t channel,
        std::function<void(uint8_t*,uint8_t)> callback) override;

    //fields for keeping track of outstanding requests for reordering
    uint64_t uniqueReqId;
    std::deque<LAMemReqState *> laPendingReqQ;

//=========================================================================
// LACore execution
//=========================================================================
    
  private:
    void startExecuteLAInsn(LAInsn& insn, LACoreSimpleExecContext *xc);
    void finishExecuteLAInsn(Fault fault);

    //the actual execution unit!
    LACoreExecUnit *laExecUnit;

};

#endif // __CPU_LA_CORE_SIMPLE_TIMING_HH__