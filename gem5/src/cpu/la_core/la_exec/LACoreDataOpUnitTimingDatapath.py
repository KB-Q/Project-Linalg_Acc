# Copyright (c) 2017 Brown University
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Samuel Steffl

from m5.params import *

from TickedObject import TickedObject

class LACoreDataOpUnitTimingDatapath(TickedObject):
    type = 'LACoreDataOpUnitTimingDatapath'
    cxx_header = "cpu/la_core/la_exec/data_op_unit_timing_datapath.hh"

    addLatencySp = Param.Cycles("Latency of add/sub sp operation")
    addLatencyDp = Param.Cycles("Latency of add/sub dp operation")
    mulLatencySp = Param.Cycles("Latency of mul sp operation")
    mulLatencyDp = Param.Cycles("Latency of mul dp operation")
    divLatencySp = Param.Cycles("Latency of div sp operation")
    divLatencyDp = Param.Cycles("Latency of div dp operation")

    #vecFuncLatency = Param.Cycles("Latency of data through vector pipeline")
    #reduceFuncLatency = Param.Cycles("Latency of data through reduce pipeline")
    #simdWidth = Param.Unsigned("width of Asrc,Bsrc,Csrc and Dst packed-bufs")
    simdWidthDp = Param.Unsigned("SIMD width in 64-bit items")
    vecNodes = Param.Unsigned("The number of parallel vector nodes")

