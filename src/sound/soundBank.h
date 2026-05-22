#pragma once

//Nome simbolico dei suoni, l'ordine deve corrispondere con soundPaths
enum class SoundID
{
    BallHitRacket,      //Palla che tocca la racchetta
    BrickDestroyed,     //Mattoncino distrutto (qualsiasi tipo)
    BrickRedHitted,     //Mattoncino rosso colpito
    BounceWall,         //Rimbalzo contro il muro
    Bonus,              //Ottiene un bonus
    LevelUp,            //Completa il livello e ne carica uno nuovo
    Lose,               //Sconfitta

    COUNT               //Tecnica per sapere la dimensione dell'enum a compile-time
};

//I file (.wav) vanno aggiunti in assets/audio/

//Elenco di path degli asset usati
static const wchar_t* soundPaths[static_cast<size_t>(SoundID::COUNT)] =
{
    L"assets\\sound\\bounce-racket.wav",    //SoundID::BallHitRacket
    L"assets\\sound\\brick-broken.wav",     //SoundID::BrickDestroyed
    L"assets\\sound\\brick-red-hitted.wav", //SoundID::BrickRedHitted
    L"assets\\sound\\bounce-wall.wav",      //SoundID::BounceWall
    L"assets\\sound\\bonus.wav",            //SoundID::Bonus
    L"assets\\sound\\level-up.wav",         //SoundID::LevelUp
    L"assets\\sound\\lose.wav",             //SoundID::Lost

};