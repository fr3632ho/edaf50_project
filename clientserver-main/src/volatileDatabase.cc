#include "volatileDatabase.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using std::map;
using std::pair;
using std::string;
using std::vector;

VolatileDatabase::VolatileDatabase() {}

bool VolatileDatabase::createNewsgroup(string title)
{
    for (auto it = newsgroups.begin(); it != newsgroups.end(); ++it)
    {
        if (it->second.getTitle() == title)
        {
            return false;
        }
    }
    newsgroups.insert(std::pair<int, Newsgroup>(id, Newsgroup(title, id)));
    id += 1;
    return true;
}

map<int, Newsgroup> VolatileDatabase::getNewsgroups()
{
    return newsgroups;
}

bool VolatileDatabase::deleteNewsgroup(int newsgroupId)
{
    size_t erased = newsgroups.erase(newsgroupId);
    // 0 if nothing erased
    return erased;
}

map<int, Article> VolatileDatabase::getNewsgroupArticles(int newsgroupId)
{
    Newsgroup &ng = newsgroups.at(newsgroupId);
    return ng.getArticles();
}

Article VolatileDatabase::getArticle(int newsgroupId, int articleId)
{
    return newsgroups.at(newsgroupId).getArticles().at(articleId);
}

void VolatileDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{
    Newsgroup &ng = newsgroups.at(newsgroupId);
    ng.writeArticle(title, text, author);
}

void VolatileDatabase::deleteArticle(int newsgroupId, int articleId)
{
    Newsgroup &ng = newsgroups.at(newsgroupId);
    ng.deleteArticle(articleId);
}
