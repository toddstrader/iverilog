/*
 * Copyright (c) 2011 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

# include  "vtype.h"
# include  "parse_types.h"
# include  <map>
# include  <typeinfo>

using namespace std;


VType::~VType()
{
}

void VType::show(ostream&out) const
{
      out << typeid(*this).name();
}

VTypePrimitive::VTypePrimitive(VTypePrimitive::type_t tt)
: type_(tt)
{
}

VTypePrimitive::~VTypePrimitive()
{
}

void VTypePrimitive::show(ostream&out) const
{
      switch (type_) {
	  case BOOLEAN:
	    out << "BOOLEAN";
	    break;
	  case BIT:
	    out << "BIT";
	    break;
	  case INTEGER:
	    out << "INTEGER";
	    break;
	  case STDLOGIC:
	    out << "std_logic";
	    break;
      }
}

VTypeArray::VTypeArray(const VType*element, const vector<VTypeArray::range_t>&r, bool sv)
: etype_(element), ranges_(r), signed_flag_(sv)
{
}

/*
 * Create a VTypeArray range set from a list of parsed ranges.
 * FIXME: We are copying pointers from the prange_t object into the
 * range_t. This means that we cannot delete the prange_t object
 * unless we invent a way to remove the pointers from that object. So
 * this is a memory leak. Something to fix.
 */
VTypeArray::VTypeArray(const VType*element, std::list<prange_t*>*r, bool sv)
: etype_(element), ranges_(r->size()), signed_flag_(sv)
{
      for (size_t idx = 0 ; idx < ranges_.size() ; idx += 1) {
	    prange_t*curp = r->front();
	    r->pop_front();
	    Expression*msb = curp->msb();
	    Expression*lsb = curp->lsb();
	    ranges_[idx] = range_t(msb, lsb);
      }
}


VTypeArray::~VTypeArray()
{
}

size_t VTypeArray::dimensions() const
{
      return ranges_.size();
}

const VType* VTypeArray::element_type() const
{
      return etype_;
}

void VTypeArray::show(ostream&out) const
{
      out << "array ";
      for (vector<range_t>::const_iterator cur = ranges_.begin()
		 ; cur != ranges_.end() ; ++cur) {
	    out << "(";
	    if (cur->msb())
		  cur->msb()->write_to_stream(out);
	    else
		  out << "<>";
	    out << " downto ";
	    if (cur->lsb())
		  cur->lsb()->write_to_stream(out);
	    else
		  out << "<>";
	    out << ")";
      }
      out << " of ";
      if (signed_flag_)
	    out << "signed ";
      if (etype_)
	    etype_->show(out);
      else
	    out << "<nil>";
}

VTypeRange::VTypeRange(const VType*base, int64_t max_val, int64_t min_val)
: base_(base)
{
      max_ = max_val;
      min_ = min_val;
}

VTypeRange::~VTypeRange()
{
}

VTypeEnum::VTypeEnum(const std::list<perm_string>*names)
: names_(names->size())
{
      size_t idx = 0;

      for (list<perm_string>::const_iterator cur = names->begin()
		 ; cur != names->end() ; ++cur, ++idx) {
	    names_[idx] = *cur;
      }
}

VTypeEnum::~VTypeEnum()
{
}

void VTypeEnum::show(ostream&out) const
{
      out << "(";
      if (names_.size() >= 1)
	    out << names_[0];
      for (size_t idx = 1 ; idx < names_.size() ; idx += 1)
	    out << ", " << names_[idx];
      out << ")";
}

VTypeRecord::VTypeRecord(std::list<element_t*>*elements)
: elements_(elements->size())
{
      for (size_t idx = 0 ; idx < elements_.size() ; idx += 1) {
	    elements_[idx] = elements->front();
	    elements->pop_front();
      }
      delete elements;
}

VTypeRecord::~VTypeRecord()
{
      for (size_t idx = 0 ; idx < elements_.size() ; idx += 1)
	    delete elements_[idx];
}

void VTypeRecord::show(ostream&out) const
{
      write_to_stream(out);
}

VTypeRecord::element_t::element_t(perm_string name, const VType*typ)
: name_(name), type_(typ)
{
}

VTypeDef::VTypeDef(perm_string nam, const VType*typ)
: name_(nam), type_(typ)
{
}

VTypeDef::~VTypeDef()
{
}
