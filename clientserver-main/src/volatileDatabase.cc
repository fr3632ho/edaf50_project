#include "volatileDatabase.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>

using std::pair;
using std::string;
using std::vector;

VolatileDatabase::VolatileDatabase() {}

void VolatileDatabase::createNewsgroup(string title)
{
    newsgroups.insert(std::pair<int, Newsgroup>(id, Newsgroup(title, id)));
    id += 1;
}

map<int, Newsgroup> VolatileDatabase::getNewsgroups()
{
    return newsgroups;
}

string VolatileDatabase::deleteNewsgroup(int newsgroupId)
{
    newsgroups.erase(newsgroupId);
    return "";
}

map<int, Article> VolatileDatabase::getNewsgroupArticles(int newsgroupId)
{
    Newsgroup &ng = newsgroups.at(newsgroupId);
    return ng.getArticles();
}

string VolatileDatabase::getArticle(int newsgroupId, int articleId)
{
    return "";
}

void VolatileDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{
    Newsgroup &ng = newsgroups.at(newsgroupId);
    ng.writeArticle(title, text, author);

    for (std::pair<int, Article> a : ng.getArticles())
    {
        std::cout << a.second.getText() << " hi" << std::endl;
    }
}

void VolatileDatabase::deleteArticle(int articleId, int newsgroupId)
{
}
