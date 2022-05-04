#include "diskDatabase.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdlib.h>
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
        std::ofstream outfile(dir);
        // o << "Hello, World!" << std::endl;
        outfile.close();
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

bool DiskDatabase::createNewsgroup(string title)
{
    
    string ng_dir = path + std::to_string(id);
    if (!fs::exists(ng_dir)) 
    {
        std::ofstream outfile;
        outfile.open(path + idmap, std::ios_base::app);
        outfile << id << " " << title << std::endl;
        outfile.close();
        id++;

        fs::create_directory(ng_dir);
        return true;
    }
    return false;
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


bool DiskDatabase::deleteNewsgroup(int newsgroupId)
{
    // Delete subdir with id
    string ng_dir = path + std::to_string(newsgroupId);
    
    if (!fs::remove(ng_dir)) 
    {
        return false;
    }

    id_newsgroup_map.erase(newsgroupId);

    std::ofstream idmap_outfile;
    idmap_outfile.open(path + idmap, std::ios::out);
    for (auto const& [key, value] : id_newsgroup_map)
    {
        std::cout << "Pairs: " << key << " " << value << std::endl;
        idmap_outfile << key << " " << value << std::endl;
    }
    idmap_outfile.close();
    return true;
}

map<int, Article> DiskDatabase::getNewsgroupArticles(int newsgroupId)
{

    Newsgroup ng = getNewsgroup(newsgroupId);
    return ng.getArticles();
}

Article DiskDatabase::getArticle(int newsgroupId, int articleId)
{
    string a_dir = path + std::to_string(newsgroupId) + "/" + std::to_string(articleId);
    string line;
    vector<string> lines;
    std::ifstream file(a_dir);
    if (file.is_open())
    {
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        Article a(lines[0], lines[1], lines[2], articleId);
        file.close();
        return a;
    }
}

void DiskDatabase::writeArticle(int newsgroupId, string title, string text, string author)
{   
    string ng_dir = path + std::to_string(newsgroupId);
    if (!fs::exists(ng_dir)) 
    {
        int id = std::rand();
        string a_dir = path + std::to_string(newsgroupId) + "/" + std::to_string(id);
        std::ofstream a_outfile(a_dir);
        a_outfile << title << std::endl;
        a_outfile << text << std::endl;
        a_outfile << author << std::endl;
        a_outfile.close();

    }
}

void DiskDatabase::deleteArticle(int newsgroupId, int articleId)
{
    string a_dir = path + std::to_string(newsgroupId) + "/" + std::to_string(articleId);
    fs::remove(a_dir);
}
