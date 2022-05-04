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
    string deleteNewsgroup(int newsgroupId) override;
    map<int, Article> getNewsgroupArticles(int newsgroupId) override;
    Article getArticle(int articleId, int newsgroupId) override;
    void writeArticle(int newsgroupId, string title, string text, string author) override;
    void deleteArticle(int articleId, int newsgroupId) override;

private:
    map<int, Newsgroup> newsgroups;
    int id{1};
};
#endif