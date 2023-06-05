#include "stream_reassembler.hh"
#include <cassert>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) :_window(),_assembled_idx(0),_unassembled_number(0), _eof_idx(-1),  _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    auto pos_iter = _window.upper_bound(index);

    if (pos_iter != _window.begin())
        pos_iter--;  //the pos_iter will 1.<= index or 2. > index

    size_t new_idx = index;

    if (pos_iter != _window.end() && pos_iter->first <= index) { //There is a string at the left of the index
        const size_t left_idx = pos_iter->first; 
        if (left_idx + pos_iter->second.size() > index)
            new_idx = left_idx + pos_iter->second.size();
    }
    //There are no strings at the left of the index
    else if (index < _assembled_idx) { 
        new_idx = _assembled_idx;
    }

    const size_t data_begin = new_idx - index;
    ssize_t data_size = data.size() - data_begin;

    if (pos_iter != _window.end() && pos_iter ->first <= new_idx)
        pos_iter++;  //Checking the string at the right of the index;
    
    while (pos_iter != _window.end() && new_idx <= pos_iter->first) {
        size_t right_idx = pos_iter->first;
        if (new_idx + data_size > right_idx) {  // The overlap happen
            if (new_idx + data_size < right_idx + pos_iter->second.size()) {//it's a partial overlap
                    data_size = right_idx - new_idx; //cut off the string and update the data size
                    break;
            }else { // the index overlap the whole right string, thus we discard the right string and check the next one
                _unassembled_number -= pos_iter->second.size();
                pos_iter = _window.erase(pos_iter);
                continue;
            }
        }
        else
            break;
    }

    size_t max_unassembled_idx = _capacity - _output.buffer_size() + _assembled_idx;

    if (new_idx >= max_unassembled_idx)
        return;

    if (data_size > 0) {
        const string new_data = data.substr(data_begin,data_size); // Create a new substring

        if (new_idx == _assembled_idx) { // If this substring can be written
            const size_t write_size = _output.write(new_data);
            _assembled_idx += write_size;
            //if the substring isn't be written totally
            if (write_size < new_data.size()) {
                const string remain_data = new_data.substr(write_size, new_data.size() - write_size); //Take the remaining substring
                _unassembled_number += remain_data.size();
                _window.insert(make_pair(_assembled_idx, std::move(remain_data))); //Store it into the window;
            }
        }else { // the data can't be written right now
            const string remain_data = new_data;
            _unassembled_number += remain_data.size();
            _window.insert(make_pair(new_idx, std::move(remain_data)));
        }
    }

    for (auto iter = _window.begin(); iter != _window.end(); ) {
        if (iter->first == _assembled_idx) {
            const size_t write_byte = _output.write(iter->second);
            _assembled_idx += write_byte;
            // the substring in the window isn't be written totally
            if (write_byte < iter->second.size()) {
                _unassembled_number += iter->second.size() - write_byte;
                _window.insert(make_pair(_assembled_idx, std::move(iter->second.substr(write_byte)))); //replace the substring with new, updated substring
                _unassembled_number -= iter->second.size();  //remove the old one
                iter = _window.erase(iter);
                break;
            }
            //the substring in window has been written totally into the bytestream
            //thus we need to remove the substring in the window;
            _unassembled_number -= iter->second.size();
            iter = _window.erase(iter); 
        }else
            break;
    }
    if (eof) {
        _eof_idx = index + data.size();
    }
    if (_eof_idx <= _assembled_idx)
        _output.end_input();    
}


size_t StreamReassembler::unassembled_bytes() const {return _unassembled_number;} 

bool StreamReassembler::empty() const { 
    return _unassembled_number == 0;
 }
