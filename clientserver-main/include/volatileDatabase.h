#ifndef VOLATILEDATABASE_H
#define VOLATILEDATABASE_H

#include "article.h"
#include "database.h"
#include "newsgroup.h"
#include <vector>
#include <string>
#include <map>

using std::map;
using std::string;
using std::vector;

class VolatileDatabase : public Database
{
public:
    VolatileDatabase();
    ~VolatileDatabase() = default;
    map<int, Newsgroup> getNewsgroups() override;
    bool createNewsgroup(string title) override;
    bool deleteNewsgroup(int newsgroupId) override;
    map<int, Article> getNewsgroupArticles(int newsgroupId) override;
    Article getArticle(int newsgroupId, int articleId) override;
    bool writeArticle(int newsgroupId, string title, string text, string author) override;
    int deleteArticle(int newsgroupId, int articleId) override;

private:
    map<int, Newsgroup> newsgroups;
    int id{1};
};
#endif