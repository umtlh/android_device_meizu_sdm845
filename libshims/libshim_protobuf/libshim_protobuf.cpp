/*
 * Copyright (C) 2020 DOLBAYOBI
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <climits>
#include <string>
#include <libshim_protobuf.h>

std::string fixed_address_empty_string() {
    return _ZN6google8protobuf8internal26fixed_address_empty_stringE;
}

extern "C" void _ZNK6google8protobuf11MessageLite31SerializeWithCachedSizesToArrayEPh(
        bool deterministic, uint8_t* target);

extern "C" void _ZNK6google8protobuf11MessageLite39InternalSerializeWithCachedSizesToArrayEPh(
        bool deterministic, uint8_t* target)
{
    return _ZNK6google8protobuf11MessageLite31SerializeWithCachedSizesToArrayEPh(deterministic, target);
}

extern "C" void _ZN6google8protobuf8internal14DestroyMessageEPKv() {}

extern "C" void _ZN6google8protobuf8internal11InitSCCImplEPNS1_11SCCInfoBaseE() {}

extern "C" void _ZN6google8protobuf8internal13OnShutdownRunEPFvPKvES3_() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream27ReadVarintSizeAsIntFallbackEv() {}

extern "C" void _ZN6google8protobuf2io18StringOutputStreamC1EPNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEE() {}

extern "C" void _ZN6google8protobuf2io17CodedOutputStreamC1EPNS1_20ZeroCopyOutputStreamEb() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite9SkipFieldEPNS0_2io16CodedInputStreamEjPNS3_17CodedOutputStreamE() {}

extern "C" void _ZN6google8protobuf2io17CodedOutputStreamD1Ev() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite12WriteMessageEiRKNS0_11MessageLiteEPNS0_2io17CodedOutputStreamE() {}

extern "C" void _ZN6google8protobuf2io17CodedOutputStream8WriteRawEPKvi() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite23WriteStringMaybeAliasedEiRKNSt3__112basic_stringIcNS3_11char_traitsIcEENS3_9allocatorIcEEEEPNS0_2io17CodedOutputStreamE() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite10WriteInt64EixPNS0_2io17CodedOutputStreamE() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite9WriteEnumEiiPNS0_2io17CodedOutputStreamE() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite10WriteInt32EiiPNS0_2io17CodedOutputStreamE() {}

extern "C" void _ZN6google8protobuf8internal13VerifyVersionEiiPKc() {}

extern "C" void _ZN6google8protobuf8internal20InitProtobufDefaultsEv() {}

extern "C" void _ZN6google8protobuf8internal9ArenaImpl28AllocateAlignedAndAddCleanupEjPFvPvE() {}

extern "C" void _ZNK6google8protobuf5Arena17OnArenaAllocationEPKSt9type_infoj() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream15ReadTagFallbackEj() {}

extern "C" void _ZN6google8protobuf8internal14WireFormatLite9ReadBytesEPNS0_2io16CodedInputStreamEPNSt3__112basic_stringIcNS6_11char_traitsIcEENS6_9allocatorIcEEEE() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream20ReadVarint64FallbackEv() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream20ReadVarint32FallbackEj() {}

extern "C" void _ZN6google8protobuf2io17CodedOutputStream21WriteVarint32SlowPathEj() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream35IncrementRecursionDepthAndPushLimitEi() {}

extern "C" void _ZN6google8protobuf8internal20RepeatedPtrFieldBase14InternalExtendEi() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream34DecrementRecursionDepthAndPopLimitEi() {}

extern "C" void _ZN6google8protobuf8internal9ArenaImpl15AllocateAlignedEj() {}

extern "C" void _ZN6google8protobuf8internal9ArenaImpl10AddCleanupEPvPFvS3_E() {}

extern "C" void _ZN6google8protobuf2io16CodedInputStream12SkipFallbackEii() {}
