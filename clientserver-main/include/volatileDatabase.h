#ifndef VOLATILEDATABASE_H
#define VOLATILEDATABASE_H

#include "database.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

class VolatileDatabase : public Database
{
public:
    VolatileDatabase();
    ~VolatileDatabase() = default;
    vector<string> getNewsgroups() override;
    void createNewsgroup(string title) override;
    string deleteNewsgroup(int newsgroupId) override;
    string getNewsgroupArticles(int newsgroupId) override;
    string getArticle(int articleId) override;
    void writeArticle(int articleId, string title, string text, string author) override;
    void deleteArticle(int articleId) override;

private:
    vector<string> newsgroups;
};
#endif