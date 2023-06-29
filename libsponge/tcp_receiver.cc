#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    const TCPHeader &header = seg.header();
    if(header.syn) {  //check if this segment carry syn
        if(synAlready_flag) //check if there was syn before
            return; //if true,return
        isn = header.seqno; 
        synAlready_flag = true;
    }
    uint64_t ackno = _reassembler.stream_out().bytes_written() + 1; //set the ack number
    uint64_t absSeqno = unwrap(header.seqno,isn,ackno);  //unwrap the seqno, translate it into 64bit (in order to get stream index)
    uint64_t stream_index = absSeqno -1 + (header.syn); 

    _reassembler.push_substring(seg.payload().copy(),stream_index,header.fin); //push the substring 
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!synAlready_flag) //check  which state are we at
        return nullopt; //no syn, no ackno
    uint64_t abs_ackno = _reassembler.stream_out().bytes_written() + 1;
    if (_reassembler.stream_out().input_ended()) //check if we have isn,if so, we need to plus 1bit
        ++abs_ackno;
    return wrap(abs_ackno,isn);
}

size_t TCPReceiver::window_size() const { return _capacity - _reassembler.stream_out().buffer_size();}
