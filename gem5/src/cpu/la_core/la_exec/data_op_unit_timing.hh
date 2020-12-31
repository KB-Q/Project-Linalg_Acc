/*
 * Copyright (c) 2017 Brown University
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
 * Authors: Samuel Steffl
 */

#ifndef __CPU_LA_CORE_LA_EXEC_DATA_OP_UNIT_TIMING_HH__
#define __CPU_LA_CORE_LA_EXEC_DATA_OP_UNIT_TIMING_HH__

#include <cstdint>
#include <deque>
#include <functional>

#include "cpu/la_core/exec_context.hh"
#include "cpu/la_core/la_exec/data_op_unit.hh"
#include "cpu/la_core/la_exec/data_op_unit_timing_datapath.hh"
#include "cpu/la_core/la_insn.hh"
#include "params/LACoreDataOpUnitTiming.hh"
#include "sim/faults.hh"
#include "sim/sim_object.hh"

class LACoreDataOpUnitTiming : public LACoreDataOpUnit 
{
  public:
    LACoreDataOpUnitTiming(const LACoreDataOpUnitTimingParams *p);
    ~LACoreDataOpUnitTiming();

    bool isOccupied() override;
    void issue(LAInsn& insn, LACoreExecContext *xc,
        std::function<void(Fault fault)> done_callback) override;

    //internal state for current instruction
    //these need to be public so Datapath can access them
    std::deque<uint8_t*> AdataQ;
    std::deque<uint8_t*> BdataQ;
    std::deque<uint8_t*> CdataQ;

  private:
    //configuration from python object
    LACoreDataOpUnitTimingDatapath * dataPath;
    
    //internal state for current instruction
    LACount Aread;    
    LACount Bread;
    LACount Cread;
    LACount Dread;
};

#endif // __CPU_LA_CORE_LA_EXEC_DATA_OP_UNIT_TIMING_HH__
