#ifndef ARTICLE_H
#define ARTICLE_H

#include <string>
#include <vector>
#include <map>

using std::map;
using std::string;
using std::vector;

// TODO: Create newsgroup class? primitive obsession bloater?
class Article
{
public:
    Article(string title, string author, string text, int id);
    Article();
    string getTitle();
    string getText();
    string getAuthor();
    int getId();

private:
    string author;
    string text;
    string title;
    int id;
};

#endif