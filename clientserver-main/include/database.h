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
    virtual bool createNewsgroup(string title) = 0;
    virtual bool deleteNewsgroup(int newsgroupId) = 0;
    virtual map<int, Article> getNewsgroupArticles(int newsgroupId) = 0;
    virtual Article getArticle(int articleId, int newsgroupId) = 0;
    virtual void writeArticle(int newsgroupId, string title, string text, string author) = 0;
    virtual void deleteArticle(int articleId, int newsgroupId) = 0;
};

#endif