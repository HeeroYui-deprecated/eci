/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/Enum.h>
#include <eci/debug.h>

void eci::Enum::addValue(const std::string& _name) {
	if (m_values.size() == 0) {
		m_values.push_back(std::make_pair(_name, 0));
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
	m_values.push_back(std::make_pair(_name, lastValue+1));
}

void eci::Enum::addValue(const std::string& _name, int32_t _value) {
	if (m_values.size() == 0) {
		m_values.push_back(std::make_pair(_name, _value));
		return;
	}
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].first == _name) {
			ECI_ERROR("Enum name already exist ... : " << _name);
			return;
		}
	}
	m_values.push_back(std::make_pair(_name, _value));
}

int32_t eci::Enum::getValue(const std::string& _name) const {
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].first == _name) {
			return m_values[iii].second;
		}
	}
	ECI_ERROR("Enum name does not exist ... : '" << _name << "'");
	return 0;
}

const std::string& eci::Enum::getName(int32_t _value) const {
	for (size_t iii=0; iii<m_values.size(); ++iii) {
		if (m_values[iii].second == _value) {
			return m_values[iii].first;
		}
	}
	ECI_ERROR("Enum name does not exist ... : '" << _value << "'");
	static const std::string errorValue = "---UnknowName---";
	return errorValue;
}

