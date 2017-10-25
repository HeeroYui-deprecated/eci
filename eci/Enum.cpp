/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/Enum.hpp>
#include <eci/debug.hpp>

void eci::Enum::addValue(const etk::String& _name) {
	if (m_values.size() == 0) {
		m_values.pushBack(etk::makePair(_name, 0));
		return;
	}
	int32_t lastValue = 0;
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].first == _name) {
			ECI_ERROR("Enum name already exist ... : " << _name);
			return;
		}
		lastValue = m_values[iii].second;
	}
	m_values.pushBack(etk::makePair(_name, lastValue+1));
}

void eci::Enum::addValue(const etk::String& _name, int32_t _value) {
	if (m_values.size() == 0) {
		m_values.pushBack(etk::makePair(_name, _value));
		return;
	}
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].first == _name) {
			ECI_ERROR("Enum name already exist ... : " << _name);
			return;
		}
	}
	m_values.pushBack(etk::makePair(_name, _value));
}

int32_t eci::Enum::getValue(const etk::String& _name) const {
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].first == _name) {
			return m_values[iii].second;
		}
	}
	ECI_ERROR("Enum name does not exist ... : '" << _name << "'");
	return 0;
}

const etk::String& eci::Enum::getName(int32_t _value) const {
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].second == _value) {
			return m_values[iii].first;
		}
	}
	ECI_ERROR("Enum name does not exist ... : '" << _value << "'");
	static const etk::String errorValue = "---UnknowName---";
	return errorValue;
}

