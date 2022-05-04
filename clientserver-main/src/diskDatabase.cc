#include "diskDatabase.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>
//#include <sys/stat.h>

using std::pair;
using std::string;
using std::system;
using std::vector;
namespace fs = std::filesystem;

const string idmap = "id_newsgroups_map.txt";

DiskDatabase::DiskDatabase(const string &path) : path(path)
{
    initDb();
}

void DiskDatabase::initDb()
{
    if (!fs::exists(path))
    {
        fs::create_directory(path);

        string dir = path;
        dir.append(idmap);
        std::cout << dir << std::endl;
        std::fflush(stdout);
        std::ofstream o(dir);
        // o << "Hello, World!" << std::endl;
        o.close();
    }
    else
    {
        int max_id = 1;
        string line, delimiter = " ", token;
        std::ifstream file(path + idmap);
        while (getline(file, line))
        {
            vector<string> tokens;
            size_t pos = line.find(delimiter);
            int id = stoi(line.substr(0, pos));
            if (id > max_id) {
                max_id = id;
            }
            string name = line.substr(pos + 1, line.length());
            id_newsgroup_map.emplace(id, name);
        }
        this->id = max_id + 1;
    }

    // for testing
    for (const auto &entry : id_newsgroup_map)
    {
        std::cout << entry.first << " -> " << entry.second << std::endl;
    }
}

void DiskDatabase::createNewsgroup(string title)
{
    
    string ng_dir = path + std::to_string(id);
    if (!fs::exists(ng_dir)) 
    {
        std::ofstream o(path + idmap);
        o << id << " " << title << std::endl;
        o.close();
        id++;

        fs::create_directory(ng_dir);
    }
}

Newsgroup DiskDatabase::getNewsgroup(int id)
{
    Newsgroup ng(id_newsgroup_map[id], id);
    string dir = path + std::to_string(id);
    for (auto & article : fs::directory_iterator(dir)) 
    {
        string a_path = article.path().string();
        string a_name = a_path.substr(path.length(), a_path.length());
        int a_id = std::stoi(a_name);
        Article a = getArticle(id, a_id);
        ng.writeArticle(a.getTitle(), a.getText(), a.getAuthor());
    }
    return ng;
}

map<int, Newsgroup> DiskDatabase::getNewsgroups()
{
    map<int, Newsgroup> groups;
    
    for (const auto &folder : fs::directory_iterator(path))
    {
        try
        {
        string f_path = folder.path().string();
        string f_name = f_path.substr(path.length(), f_path.length());
        int id = std::stoi(f_name);
        std::cout << id << std::endl;
        Newsgroup ng = getNewsgroup(id);
        groups.emplace(id, ng);
        }
        catch(const std::exception& e)
        {
            continue;
        }
    }

    return groups;
}


string DiskDatabase::deleteNewsgroup(int newsgroupId)
{
    // Delete subdir with id
    string ng_dir = path + std::to_string(newsgroupId);
    if (!fs::remove(ng_dir)) 
    {
        return "";
    }
    return ng_dir;
}

map<int, Article> DiskDatabase::getNewsgroupArticles(int newsgroupId)
{

    // get all articles in subdir with id

    // return ng.getArticles();
}

Article DiskDatabase::getArticle(int newsgroupId, int articleId)
{
    // get certain article with id

    // return newsgroups.at(newsgroupId).getArticles().at(articleId);
}

void DiskDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{
    // write new article in subdir with id
}

void DiskDatabase::deleteArticle(int newsgroupId, int articleId)
{
    // Delete certain article
}
