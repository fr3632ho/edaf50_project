#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <vector>
#include <string>

using std::string;
using std::vector;

class Newsgroup
{
public:
    Newsgroup(string title, int id);
    string getTitle();
    int getId();

private:
    vector<string> articles;
    string title;
    int id;
};
#endif