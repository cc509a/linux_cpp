#ifndef _H_MYLIB_INI_H_
#define _H_MYLIB_INI_H_


#include "mylib_file.h"


namespace mylib
{

class MylibIni
{
public:
    typedef std::map<std::string, std::string> MapValue;
    typedef MapValue::iterator IterValue;
    typedef MapValue::const_iterator ConstIterValue;
    typedef std::map<std::string, MapValue> MapSection;
    typedef MapSection::iterator IterSection;
    typedef MapSection::const_iterator ConstIterSection;

    MylibIni(const std::string& file)
        :file_(file) {

    }

    MylibIni() {

    }

    ~MylibIni() {

    }

    std::string get(const std::string& section, const std::string& key,
                    const std::string& value) const {
        ConstIterSection iter_section = section_.find(section);
        if (iter_section != section_.end()) {
            ConstIterValue iter_value = iter_section->second.find(key);
            if (iter_value != iter_section->second.end()) {
                return iter_value->second;
            }

            return value;
        }

        return value;
    }

    int32_t get(const std::string& section, const std::string& key,
                const int32_t value) const {
        ConstIterSection iter_section = section_.find(section);
        if (iter_section != section_.end()) {
            ConstIterValue iter_value = iter_section->second.find(key);
            if (iter_value != iter_section->second.end()) {
                return atoi(iter_value->second.c_str());
            }

            return value;
        }

        return value;
    }

    uint32_t get(const std::string& section, const std::string& key,
                uint32_t value) const {
        std::string str_value(get(section, key, ""));
        if (str_value.empty()) {
            return value;
        }
        return strtol(str_value.c_str(), NULL, 10);
    }

    int64_t get(const std::string& section, const std::string& key,
                int64_t value) const {
        std::string str_value(get(section, key, ""));
        if (str_value.empty()) {
            return value;
        }
        return strtol(str_value.c_str(), NULL, 10);
    }

    uint64_t get(const std::string& section, const std::string& key,
                uint64_t value) const {
        std::string str_value(get(section, key, ""));
        if (str_value.empty()) {
            return value;
        }
        return strtoul(str_value.c_str(), NULL, 10);
    }

    // int32_t GetJsonConfig(const std::string& section, const std::string& key,
    //             Json::Value &value) const {
    //     std::string str_value(get(section, key, ""));
    //     Json::Reader reader;
    //     if (!reader.parse(str_value,value)) {
    //         return -1;
    //     }

    //     return 0;
    // }

    const MapValue& GetSectionMapValue(const std::string& section)
    {
        return section_[section];
    }

    int32_t Init(const std::string& file) {
        file_ = file;
        return Init();
    }

    int32_t Init() {
        MylibFile file(file_);

        std::vector<std::string> lines;
        IFNOT_RET(file.Read(lines), 0);

        std::string section = "";
        for (uint32_t i = 0; i < lines.size(); ++i) {
            std::string& line = lines[i];
            if (line[0] == ';' || line[0] == '#' || line.empty()) {
                continue;
            }

            if (line[0] == '[') {
                uint32_t pos_section = strcspn(line.c_str(), "]");
                if (pos_section > 0) {
                    section = line.substr(1, pos_section - 1);
                    continue;
                }
            }

            if (!section.empty()) {
                std::string::size_type pos_space = line.find_first_not_of(" ");
                line.erase(0, pos_space);

                uint32_t pos_key = strcspn(line.c_str(), " =\t");
                std::string key = line.substr(0, pos_key);

                while (line[pos_key] == ' ' ||  line[pos_key] == '=' || line[pos_key] == '\t') {
                    ++pos_key;
                }

                std::string value = line.substr(pos_key);
                section_[section].insert(make_pair(key, value));
            }
        }

        return SUCCESS;
    }

private:
    std::string file_;
    MapSection  section_;
};

}

#endif // _H_MYLIB_INI_H_




