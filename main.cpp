#include <iomanip>
#include <iostream>
#include <string>
#include "game_state.hpp"

namespace {

GameState::BulletType parseBullet(const std::string& s) {
    if (s == "live") return GameState::BulletType::Live;
    if (s == "blank") return GameState::BulletType::Blank;
    throw InvalidOperation();
}
GameState::ItemType parseItem(const std::string& s) {
    if (s == "dummy") return GameState::ItemType::Dummy;
    if (s == "magnifier") return GameState::ItemType::Magnifier;
    if (s == "converter") return GameState::ItemType::Converter;
    if (s == "cage") return GameState::ItemType::Cage;
    throw InvalidOperation();
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int t=0; if(!(std::cin>>t)) return 0;
    for(int caseId=0; caseId<t; ++caseId){
        GameState game;
        std::string cmd;
        while(game.winnerId()==-1 && (std::cin>>cmd)){
            if(cmd=="op"){
                std::string opName; std::cin>>opName;
                try{
                    if(opName=="use_cage"){
                        game.useCage();
                    }else{
                        std::string bulletText; std::cin>>bulletText;
                        auto bullet=parseBullet(bulletText);
                        if(opName=="fire_opponent") game.fireAtOpponent(bullet);
                        else if(opName=="fire_self") game.fireAtSelf(bullet);
                        else if(opName=="use_dummy") game.useDummy(bullet);
                        else if(opName=="use_magnifier") game.useMagnifier(bullet);
                        else if(opName=="use_converter") game.useConverter(bullet);
                        else throw InvalidOperation();
                    }
                }catch(const std::exception&){
                    std::cout<<"error!\n";
                }
            }else if(cmd=="query"){
                std::string what; std::cin>>what; double ans=0.0;
                if(what=="live") ans=game.nextLiveBulletProbability();
                else ans=game.nextBlankBulletProbability();
                std::cout<<std::fixed<<std::setprecision(2)<<ans<<'\n';
            }else if(cmd=="supply"){
                std::string kind; std::cin>>kind;
                if(kind=="bullet"){
                    int l=0,b=0; std::cin>>l>>b; game.reloadBullets(l,b);
                }else if(kind=="item"){
                    int id=0; std::string itemName; std::cin>>id>>itemName;
                    game.reloadItem(id, parseItem(itemName));
                }
            }else{
                throw InvalidOperation();
            }
            const int winner=game.winnerId();
            if(winner!=-1){
                std::cout<<"player "<<winner<<" win!\n";
                break;
            }
        }
    }
    return 0;
}
