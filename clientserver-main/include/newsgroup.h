#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include "article.h"
#include <vector>
#include <string>
#include <map>

using std::map;
using std::string;
using std::vector;

class Newsgroup
{
public:
    Newsgroup(string title, int id);
    string getTitle();
    map<int, Article> getArticles();
    void writeArticle(string title, string text, string author);
    int getId();

private:
    map<int, Article> articles;
    string title;
    int id;
    int article_id{1};
};
#endif