#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity):_buffer(), _capacity(capacity), _writeSize(0), _readSize(0), _inputOver(false), _error(false){} 

size_t ByteStream::write(const string &data) {
    if (_inputOver) {
        return 0;
    }
    size_t writingSize = min(data.size(), _capacity - _buffer.size());
    _writeSize += writingSize;
    for (size_t i = 0; i < writingSize; i++) {
        _buffer.push_back(data[i]);
    }
    return writingSize;
    
    
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t peekSize = min(len,_buffer.size());
    return string(_buffer.begin(),_buffer.begin() + peekSize);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t popSize = min(len, _buffer.size());
    _readSize += popSize;
    for (size_t i =0; i < popSize; i++) {
        _buffer.pop_front();
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string readValue = this->peek_output(len);
    this->pop_output(len);
    return readValue;
}

void ByteStream::end_input() {_inputOver = true;}

bool ByteStream::input_ended() const { return _inputOver;}

size_t ByteStream::buffer_size() const { return _buffer.size(); }

bool ByteStream::buffer_empty() const { return _buffer.empty(); }

bool ByteStream::eof() const { return _inputOver && _buffer.empty(); } //Only when there are no more inputs && the buffer is empty, the stream hit eof.

size_t ByteStream::bytes_written() const { return _writeSize; }

size_t ByteStream::bytes_read() const { return _readSize; }

size_t ByteStream::remaining_capacity() const { return _capacity - _buffer.size(); }
