///////////////////////////////////////////////////////////////////////////////
// disks.hpp
//
// Definitions for two algorithms that each solve the alternating disks
// problem.
//
// As provided, this header has four functions marked with TODO comments.
// You need to write in your own implementation of these functions.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

// State of one disk, either light or dark.
enum disk_color { DISK_DARK, DISK_LIGHT };

// Data structure for the state of one row of disks.
class disk_state {
private:
  std::vector<disk_color> _colors;

public:

  disk_state(size_t light_count)
    : _colors(light_count * 2, DISK_DARK) {

      assert(light_count > 0);

      for (size_t i = 1; i < _colors.size(); i += 2) {
        _colors[i] = DISK_LIGHT;
      }
  }

  // Equality operator for unit tests.
  bool operator== (const disk_state& rhs) const {
    return std::equal(_colors.begin(), _colors.end(), rhs._colors.begin());
  }

  size_t total_count() const {
    return _colors.size();	//sc 2
  }

  size_t light_count() const {
    return total_count() / 2;
  }

  size_t dark_count() const {
    return light_count();
  }

  bool is_index(size_t i) const {
    return (i < total_count());	//sc 4
  }

  disk_color get(size_t index) const {	//sc 6
    assert(is_index(index));	//sc 5
    return _colors[index];	//sc 1
  }

  void swap(size_t left_index) {	//sc 5+2+5+1 = 13
    assert(is_index(left_index));	//sc 4+1 = 5
    auto right_index = left_index + 1;	//sc 2
    assert(is_index(right_index));	//sc 4+1 = 5
    std::swap(_colors[left_index], _colors[right_index]);	//sc 1
  }

  std::string to_string() const {
    std::stringstream ss;
    bool first = true;
    for (auto color : _colors) {
      if (!first) {
        ss << " ";
      }

      if (color == DISK_LIGHT) {
        ss << "L";
      } else {
        ss << "D";
      }

      first = false;
    }
    return ss.str();
  }

  // Return true when this disk_state is in alternating format. That means
  // that the first disk at index 0 is dark, the second disk at index 1
  // is light, and so on for the entire row of disks.
  bool is_alternating() const 
  {//is_alternating	sc 11n+1
	//for i= 0 to n do	sc 11n
	for(size_t i = 0; i < _colors.size(); ++i)	//sc n
		{//for
		//if i % 2 == 0 do	sc 2+max(8,9) = 2+9 = 11
		if((i % 2) == 0)	//sc 2
			{//if
			//if disk[i] == light return false sc max(8,7) = 8
			if(_colors[i] == DISK_LIGHT) //sc 7
				{
				return false;	//sc 1
				}
			}//if
		//else do	sc 8+1 = 9
		else	//sc 1
			{//else
			//if disk[i] == dark return false  sc max(8,7) = 8
			if(_colors[i] == DISK_DARK)	//sc 7
				{
				return false;	//sc 1
				}
			}//else
		}//for
	//after for loop return true
	return true;	//sc 1
  }//is_alternating

  // Return true when this disk_state is fully sorted, with all light disks
  // on the right (high indices) and all dark disks on the left (low
  // indices).
  bool is_sorted() const 
  {//is_sorted	sc 10n+1+1 = 10n+2
	//starts on the dark half
	bool stillDark = true;	//sc 1
	//for i = 0 to n do	
	for(size_t i = 0; i < _colors.size() / 2; ++i)
		{//for
		//is it light? it's in the wrong side
		//if disk[i] == light return false
		if(_colors[i] == DISK_LIGHT)
			{
			//cout << "is sorted light disk on dark side return false\n";
			return false;
			}
		}//for
	for(size_t i = _colors.size() / 2; i < _colors.size(); ++i)
		{//for
		//is it dark? it's in the wrong side
		//if disk[i] == dark return false
		if(_colors[i] == DISK_DARK)
			{
			return false;
			}
		}//for
	//return true because all the tests have been passed
	return true;
  }//is_sorted
};

// Data structure for the output of the alternating disks problem. That
// includes both the final disk_state, as well as a count of the number
// of swaps performed.
class sorted_disks {
private:
  disk_state _after;
  unsigned _swap_count;

public:

  sorted_disks(const disk_state& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }

  sorted_disks(disk_state&& after, unsigned swap_count)
    : _after(after), _swap_count(swap_count) { }

  const disk_state& after() const {
    return _after;
  }

  unsigned swap_count() const {
    return _swap_count;
  }
};

// Algorithm that sorts disks using the left-to-right algorithm.
sorted_disks sort_left_to_right(const disk_state& before) 
{//sort_left_to_right

	// check that the input is in alternating format
  	assert(before.is_alternating());
	
	unsigned numSwaps = 0;

	//make a non-const set of disks
	disk_state newDisks(before);
	
	//while ! is sorted do
	while(!newDisks.is_sorted())
		{//while
		//for i = 0 to n do
		for(size_t i = 0; i < newDisks.total_count(); ++i)
			{//for
			//if current disk is light and the next is dark then swap
			//if (disk[i] == light) && (disk[i+1] == dark) do
			if((newDisks.get(i) == DISK_LIGHT)&&(newDisks.get(i+1) == DISK_DARK))
				{//if
				++numSwaps;
				
				//swap disk[i] and disk[i+1]
				newDisks.swap(i);
				}//if
			}//for
		}//while

  	return sorted_disks(newDisks, numSwaps);
}//sort_left_to_right

// Algorithm that sorts disks using the lawnmower algorithm.
sorted_disks sort_lawnmower(const disk_state& before) 
{//sort_lawnmower
  	// check that the input is in alternating format
  	assert(before.is_alternating());

	unsigned numSwaps = 0;

	//make a non-const set of disks
	disk_state newDisks(before);

	//while ! is sorted do
	while(!newDisks.is_sorted())
		{//while
		//for i = 0 to n do
		for(size_t i = 0; i < newDisks.total_count(); ++i)
			{//for
			//if current disk is light and the next is dark then swap
			//if (disk[i] == light) && (disk[i+1] == dark) do
			if((newDisks.get(i) == DISK_LIGHT)&&(newDisks.get(i+1) == DISK_DARK))
				{//if
				++numSwaps;
				
				//swap disk[i] and disk[i+1]
				newDisks.swap(i);
				}//if
			}//for
		//for i = n - 1 to 0 do
		for(size_t i = newDisks.total_count() - 1; i > 0; --i)
			{//for
			//if current disk is dark and the left is light then swap
			//if(disk[i] == dark) && (disk[i-1] == light) do
			if((newDisks.get(i) == DISK_DARK)&&(newDisks.get(i-1) == DISK_LIGHT))
				{//if
				++numSwaps;

				//swap disk[i] and disk[i-1]
				newDisks.swap(i-1);
				}//if
			}//for
		}//while

  	return sorted_disks(newDisks, numSwaps);
}//sort_lawnmower
