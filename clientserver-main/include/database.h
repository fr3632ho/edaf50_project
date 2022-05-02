#ifndef DATABASE_H
#define DATABASE_H

#include "newsgroup.h"
#include <string>
#include <vector>
#include <map>

using std::map;
using std::string;
using std::vector;

// TODO: Create newsgroup class? primitive obsession bloater?
class Database
{
public:
    virtual ~Database(){};
    virtual map<int, Newsgroup> getNewsgroups() = 0;
    virtual void createNewsgroup(string title) = 0;
    virtual string deleteNewsgroup(int newsgroupId) = 0;
    virtual string getNewsgroupArticles(int newsgroupId) = 0;
    virtual string getArticle(int articleId) = 0;
    virtual void writeArticle(int articleId, string title, string text, string author) = 0;
    virtual void deleteArticle(int articleId) = 0;
};

#endif