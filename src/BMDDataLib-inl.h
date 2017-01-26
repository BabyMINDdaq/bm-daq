/** This file is part of BabyMINDdaq software package. This software
 * package is designed for internal use for the Baby MIND detector
 * collaboration and is tailored for this use primarily.
 *
 * BabyMINDdaq is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BabyMINDdaq is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BabyMINDdaq. If not, see <http://www.gnu.org/licenses/>.
 *
 *  \author   Yordan Karadzhov <Yordan.Karadzhov \at cern.ch>
 *            University of Geneva
 *
 *  \created  Jan 2017
 */

template<typename HeaderType>
BMDMemBank<HeaderType>::BMDMemBank() : UFEDataContainer() {
  this->resize( sizeof(HeaderType) );
  header_        = reinterpret_cast<HeaderType*>(this->buffer());
  header_->size_ = this->size();
}

template<typename HeaderType>
BMDMemBank<HeaderType>::BMDMemBank(size_t s) : UFEDataContainer(s) {
  this->resize( sizeof(HeaderType) );
  header_        = reinterpret_cast<HeaderType*>(this->buffer());
  header_->size_ = this->size();
}

template<typename HeaderType>
BMDMemBank<HeaderType>::BMDMemBank(const BMDMemBank &b) : UFEDataContainer(b) {
  header_ = reinterpret_cast<HeaderType*>(this->buffer());
}

template<typename HeaderType>
BMDMemBank<HeaderType>::BMDMemBank(BMDMemBank &&b) {
  *this = std::move(b);
}

template<typename HeaderType>
BMDMemBank<HeaderType>& BMDMemBank<HeaderType>::operator=(const BMDMemBank &b) {
  UFEDataContainer::operator=(b);
  header_ = reinterpret_cast<HeaderType*>(this->buffer());
  return *this;
}

template<typename HeaderType>
BMDMemBank<HeaderType>& BMDMemBank<HeaderType>::operator=(BMDMemBank &&b) {
  if(this != &b) {
    UFEDataContainer::operator=(std::move(b));
    header_   = std::move( b.header_ );
    b.header_ = nullptr;
  }

  return *this;
}

template<typename HeaderType>
void BMDMemBank<HeaderType>::clear() {
  UFEDataContainer::clear();
  this->resize( sizeof(HeaderType) );
  header_clear(header_);
}

