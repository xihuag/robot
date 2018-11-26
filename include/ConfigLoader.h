#ifndef CONFIG_H
#define CONFIG_H

#include "global.h"


namespace GRobot{
class ConfigLoader
{
public:
    ConfigLoader(const char *filename){fileLoad(filename);};
    ~ConfigLoader(){fileFree();};


    int findSection(const char *section, char **sect1, char **sect2, char **cont1, char **cont2, char **nextsect);
    //get string
    int getString(const char *section, const char *key, char *value, int size, const char *defvalue);
    std::string getString(const std::string key,const std::string defaultValue );
    std::string getString(const std::string key);
    //get int
    int getInt(const char *section, const char *key, int defvalue);
    int getInt(const std::string key, int defvalue=0);
    //get double
    double getDouble(const char *section, const char *key, double defvalue);
    int getValue(const char *section, const char *key, char *value, int maxlen, const char *defvalue=0 );

    //write item. if it's value is null, then delete it
    int setString(const char *section, const char *key, const char *value);
    //write integer item, base: 10、16 or 8，分别表示10、16、8进制，default is 10
    int setInt(const char *section, const char *key, int value, int base=10);

    int setString(const std::string& key, const std::string& value);
    //write integer item, base: 10、16 or 8，分别表示10、16、8进制，default is 10
    int setInt(const std::string& key, const int value );

private:
    char gFilename[SIZE_FILENAME];
    char *gBuffer;
    int gBuflen;
    int fileLoad(const char *filename);
    void fileFree();
    //int iniGetIP(const char *section, const char *key, BasicHashTable *hashtable, int size, const char *defvalue);
};
}
#endif // CONFIG_H
