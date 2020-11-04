#include "Archive.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

Archive::Archive(const string &filename) : filename(filename) {
    std::ifstream file(filename);
    if(file.is_open()){
        std::string str;
        while(std::getline(file, str)){
            char discard;
            unsigned id, view_count;
            std::string title, streamer, stream_type, language, start_date, end_date;

            std::stringstream ss1{str};
            ss1 >> discard >> id >> discard;
            std::getline(ss1 >> std::ws, title);

            std::getline(file, str); std::stringstream ss2{str};
            ss2 >> str >> streamer;

            std::getline(file, str); std::stringstream ss3{str};
            ss3 >> stream_type >> language >> view_count;

            std::getline(file >> std::ws, str);
            start_date = str.substr(0,16);  end_date = str.substr(19, 16);

            streams.emplace_back(id, title, streamer, start_date, end_date, language, view_count, stream_type == "public");
        }
        file.close();
    }
}

Archive::~Archive(){
    if(!test){
        std::ofstream file(filename, std::ofstream::trunc);
        if(file.is_open()){
            for(const auto &data : streams){
                file << data;
            }
            file.close();
        }
    }
}

unsigned int Archive::getStreamCount() const {
    return streams.size();
}

void Archive::show() const{
    for(const auto &data : streams){
        data.show();
    }
}

void Archive::showTop() const{
    std::cout << "Top by Views:" << std::endl;
    int i = 1;
    for(auto it = top_views.rbegin(); it != top_views.rend(); ++it){
        std::cout << i++ << ": "; (*it).show();
    }
    // TODO add likes
}

void Archive::showStream(unsigned int id) const{
    int pos = binarySearch(id);
    if(pos != -1) {
        streams[pos].show();
    }
}

std::vector<const StreamData *> Archive::getStreamsById(const std::vector<unsigned int> &ids) const{
    std::vector<const StreamData *> vec;
    for(auto id : ids){
        int pos = binarySearch(id);
        if(pos != -1){
            vec.push_back(&streams[pos]);
        }
    }
    return vec;
}

int Archive::binarySearch(unsigned int id) const{
    int left = 0, right = streams.size()-1;
    while (left <= right) {
        int m = left + (right - left) / 2;
        unsigned int current_id = streams[m].getId();
        if (current_id == id)
            return m;
        if (current_id < id)
            left = m + 1;
        else
            right = m - 1;
    }
    return -1;
}

void Archive::archiveStream(const StreamData &data) {
    auto it = std::lower_bound(streams.begin(), streams.end(), data.getId(),
                       [](const StreamData &d, unsigned int id){
                           return d.getId() < id;
                       });
    streams.insert(it, data);
    updateTop(data);
}

void Archive::updateTop(const StreamData &data){
    auto views_it = std::lower_bound(top_views.begin(), top_views.end(), data.getViewers(),  [](const StreamData &d, unsigned int viewers){
        return d.getViewers() < viewers;
    });
    if(views_it != top_views.begin() || top_views.size() < 10){
        top_views.insert(views_it, data);
        if(top_views.size() > 10)
            top_views.pop_back();
    }
    // TODO Implement likes
    /*
    auto likes_it = std::lower_bound(top_archived.likes.begin(), top_archived.likes.end(), [data](StreamData &d){
        return d.getLikes() < data;
    });
    if(likes_it != top_archived.likes.begin()){
        top_archived.likes.insert(likes_it, data);
        top_archived.likes.pop_back();*/
}

void Archive::testMode(){
    test = true;
    top_views.clear();
    top_likes.clear();
    streams.clear();
}