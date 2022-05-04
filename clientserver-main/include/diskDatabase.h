#ifndef DISKDATABASE_H
#define DISKDATABASE_H

#include "article.h"
#include "database.h"
#include "newsgroup.h"
#include <vector>
#include <string>
#include <map>

using std::map;
using std::string;
using std::vector;

class DiskDatabase : public Database
{
public:
    DiskDatabase(const string &path = "db/");
    ~DiskDatabase() = default;
    map<int, Newsgroup> getNewsgroups() override;
    void createNewsgroup(string title) override;
    string deleteNewsgroup(int newsgroupId) override;
    map<int, Article> getNewsgroupArticles(int newsgroupId) override;
    Article getArticle(int articleId, int newsgroupId) override;
    void writeArticle(int newsgroupId, string title, string text, string author) override;
    void deleteArticle(int articleId, int newsgroupId) override;

private:
    map<int, Newsgroup> newsgroups;
    map<int, string> id_newsgroup_map;
    const string path;
    int id{1};
    void initDb();
    Newsgroup getNewsgroup(int);
};
#endif