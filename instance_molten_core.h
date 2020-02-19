/*
 * Copyright (C) 2016+     AzerothCore <www.azerothcore.org>, released under GNU GPL v2 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-GPL2
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 */

/* ScriptData
SDName: Instance_Molten_Core
SD%Complete: 0
SDComment: Place Holder
SDCategory: Molten Core
EndScriptData */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "CreatureAI.h"
#include "molten_core.h"
#include "TemporarySummon.h"

enum Spells
{
    SPELL_INCITE_FLAMES     = 19635,
    SPELL_FIRE_BLOSSOM      = 19637,
    SPELL_ROOT              = 90019,
};

enum Events
{
    EVENT_INCITE_FLAMES         = 1,
    EVENT_FIRE_BLOSSOM          = 2,
    EVENT_FIRE_BLOSSOM_CHECK    = 3,
    EVENT_CLEAR_ROOT            = 4,
};

Position const SummonPositions[10] =
{
    {759.542f, -1173.43f, -118.974f, 3.3048f},
    {761.652f, -1164.30f, -119.533f, 3.3919f},
    {747.323f, -1149.24f, -120.060f, 3.6629f},
    {766.734f, -1183.16f, -119.292f, 2.9889f},
    {757.364f, -1198.31f, -118.652f, 2.3095f},
    {752.349f, -1159.19f, -119.261f, 3.6032f},
    {738.015f, -1152.22f, -119.512f, 4.0792f},
    {757.246f, -1189.79f, -118.633f, 2.5333f},
    {745.916f, -1199.35f, -118.119f, 1.8932f},
    {838.510f, -829.840f, -232.000f, 2.00000f},
};

class instance_molten_core : public InstanceMapScript
{
    public:
        instance_molten_core() : InstanceMapScript("instance_molten_core", 409) { }

        struct instance_molten_core_InstanceMapScript : public InstanceScript
        {
            instance_molten_core_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                _golemaggTheIncineratorGUID = 0;
                _majordomoExecutusGUID = 0;
                _cacheOfTheFirelordGUID = 0;
                _deadBossCount = 0;
                _ragnarosAddDeaths = 0;
            }

            void OnPlayerEnter(Player* /*player*/) override
            {
                if (CheckMajordomoExecutus())
                    SummonMajordomoExecutus();
            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case NPC_GOLEMAGG_THE_INCINERATOR:
                        _golemaggTheIncineratorGUID = creature->GetGUID();
                        break;
                    case NPC_MAJORDOMO_EXECUTUS:
                        _majordomoExecutusGUID = creature->GetGUID();
                        break;
                    case NPC_FLAMEWAKER:
                        gehennasFlamewakers.push_back(creature->GetGUID());
                        break;
                    case NPC_FLAMEWAKER_PROTECTOR:
                    case NPC_FLAMEWAKER_GUARDIAN:
                        lucifronFlamewakers.push_back(creature->GetGUID());
                        break;
                    case NPC_FLAMEWAKER_PRIEST:
                        sulfuronFlamewakers.push_back(creature->GetGUID());
                        break;
                    case NPC_LUCIFRON:
                        _lucifronGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go) override
            {
                switch (go->GetEntry())
                {
                    case GO_CACHE_OF_THE_FIRELORD:
                        _cacheOfTheFirelordGUID = go->GetGUID();
                        break;
                    case GO_CIRCLE_BARON:
                        OnCreateFireCircleObject(BOSS_BARON_GEDDON, go);
                        break;
                    case GO_CIRCLE_GARR:
                        OnCreateFireCircleObject(BOSS_GARR, go);
                        break;
                    case GO_CIRCLE_GEHENNAS:
                        OnCreateFireCircleObject(BOSS_GEHENNAS, go);
                        break;
                    case GO_CIRCLE_GOLEMAGG:
                        OnCreateFireCircleObject(BOSS_GOLEMAGG_THE_INCINERATOR, go);
                        break;
                    case GO_CIRCLE_MAGMADAR:
                        OnCreateFireCircleObject(BOSS_MAGMADAR, go);
                        break;
                    case GO_CIRCLE_SHAZZRAH:
                        OnCreateFireCircleObject(BOSS_SHAZZRAH, go);
                        break;
                    case GO_CIRCLE_SULFURON:
                        OnCreateFireCircleObject(BOSS_SULFURON_HARBINGER, go);
                        break;
                    case GO_RUNE_SULFURON:
                        OnCreateRuneObject(BOSS_SULFURON_HARBINGER, go);
                        break;
                    case GO_RUNE_BARON:
                        OnCreateRuneObject(BOSS_BARON_GEDDON, go);
                        break;
                    case GO_RUNE_SHAZZRAH:
                        OnCreateRuneObject(BOSS_SHAZZRAH, go);
                        break;
                    case GO_RUNE_GOLEMAGG:
                        OnCreateRuneObject(BOSS_GOLEMAGG_THE_INCINERATOR, go);
                        break;
                    case GO_RUNE_GARR:
                        OnCreateRuneObject(BOSS_GARR, go);
                        break;
                    case GO_RUNE_MAGMADAR:
                        OnCreateRuneObject(BOSS_MAGMADAR, go);
                        break;
                    case GO_RUNE_GEHENNAS:
                        OnCreateRuneObject(BOSS_GEHENNAS, go);
                        break;
                    default:
                        break;
                }
            }

            void OnCreateFireCircleObject(Encounters bossId, GameObject* circle)
            {
                if (_runesStatus[bossId] == RUNE_USED)
                {
                    circle->Delete();
                }
            }

            void OnCreateRuneObject(Encounters bossId, GameObject* rune)
            {
                _runesGUID[bossId] = rune->GetGUID();
                if (GetBossState(bossId) == DONE && _runesStatus[bossId] == RUNE_UNUSED)
                {
                    rune->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }
                if (_runesStatus[bossId] == RUNE_USED)
                {
                    rune->SetGoState(GO_STATE_ACTIVE);
                }
            }

            bool UseRune(Encounters bossId, GameObject* rune)
            {
                if (GetBossState(bossId) == DONE && _runesStatus[bossId] == RUNE_UNUSED)
                {
                    rune->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    _runesStatus[bossId] = RUNE_USED;
                    InstanceScript::SaveToDB();
                    if (CheckMajordomoExecutus())
                    {
                        SummonMajordomoExecutus();
                    }
                    return true;
                }
                return false;
            }

            void SetData(uint32 type, uint32 data) override
            {
                if (type == DATA_RAGNAROS_ADDS)
                {
                    if (data == 1)
                        ++_ragnarosAddDeaths;
                    else if (data == 0)
                        _ragnarosAddDeaths = 0;
                }
            }

            uint32 GetData(uint32 type) const  override
            {
                switch (type)
                {
                    case DATA_RAGNAROS_ADDS:
                        return _ragnarosAddDeaths;
                }

                return 0;
            }

            uint64 GetData64(uint32 type) const  override
            {
                switch (type)
                {
                    case BOSS_GOLEMAGG_THE_INCINERATOR:
                        return _golemaggTheIncineratorGUID;
                    case BOSS_MAJORDOMO_EXECUTUS:
                        return _majordomoExecutusGUID;
                    case BOSS_LUCIFRON:
                        return _lucifronGUID;
                }

                return 0;
            }

            bool SetBossState(uint32 bossId, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(bossId, state))
                    return false;

                if (state == DONE)
                {
                    if (_runesGUID[bossId])
                    {
                        if (GameObject* rune = instance->GetGameObject(_runesGUID[bossId]))
                        {
                            rune->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        }
                    }
                }
                
                if (bossId == BOSS_MAJORDOMO_EXECUTUS && state == DONE)
                {
                    DoRespawnGameObject(_cacheOfTheFirelordGUID, 7 * DAY);
                }

                if (bossId == BOSS_GEHENNAS && state == NOT_STARTED)
                {
                    for (uint64 &guid : gehennasFlamewakers)
                    {
                        if (Creature* guard = instance->GetCreature(guid))
                        {
                            if (!guard->IsAlive())
                            {
                                guard->SetPosition(guard->GetHomePosition());
                                guard->Respawn();
                            }
                        }
                    }
                }

                if (bossId == BOSS_LUCIFRON && state == NOT_STARTED)
                {
                    for (uint64 &guid : lucifronFlamewakers)
                    {
                        if (Creature* guard = instance->GetCreature(guid))
                        {
                            if (!guard->IsAlive())
                            {
                                guard->SetPosition(guard->GetHomePosition());
                                guard->Respawn();
                            }
                        }
                    }
                }

                if (bossId == BOSS_SULFURON_HARBINGER && state == NOT_STARTED)
                {
                    for (uint64 &guid : sulfuronFlamewakers)
                    {
                        if (Creature* guard = instance->GetCreature(guid))
                        {
                            if (!guard->IsAlive())
                            {
                                guard->SetPosition(guard->GetHomePosition());
                                guard->Respawn();
                            }
                        }
                    }
                }

                return true;
            }

            void SummonMajordomoExecutus()
            {
                if (_majordomoExecutusGUID)
                    return;

                if (GetBossState(BOSS_MAJORDOMO_EXECUTUS) != DONE)
                {
                    TempSummon* majordomo = instance->SummonCreature(NPC_MAJORDOMO_EXECUTUS, SummonPositions[0]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[1]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[2]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[3]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[4]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[5]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[6]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[7]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[8]);
                    majordomo->MonsterYell("The runes of warding have been destroyed! Hunt down the infidels, my brethren!", LANG_UNIVERSAL, 0);
                }
                else if (TempSummon* summon = instance->SummonCreature(NPC_MAJORDOMO_EXECUTUS, RagnarosTelePos))
                    summon->AI()->DoAction(ACTION_START_RAGNAROS_ALT);
            }

            bool CheckMajordomoExecutus() const
            {
                if (GetBossState(BOSS_RAGNAROS) == DONE)
                    return false;
                
                for (int i = 1; i < 8; i++)
                {
                    if (_runesStatus[i] != RUNE_USED)
                    {
                        return false;
                    }
                }

                return true;
            }

            std::string GetSaveData()  override
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "M C " << GetBossSaveData();
                for (int i = 1; i < 8; i++)
                {
                    saveStream << (uint32)_runesStatus[i] << ' ';
                }

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* data)  override
            {
                if (!data)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(data);

                char dataHead1, dataHead2;

                std::istringstream loadStream(data);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'M' && dataHead2 == 'C')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > TO_BE_DECIDED)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }

                    for (uint8 i = 1; i < 8; i++)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        _runesStatus[i] = MoltenCoreRuneStatus(tmpState);
                    }

                    if (CheckMajordomoExecutus())
                        SummonMajordomoExecutus();
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }

        private:
            uint64 _golemaggTheIncineratorGUID;
            uint64 _majordomoExecutusGUID;
            uint64 _cacheOfTheFirelordGUID;
            uint64 _lucifronGUID;
            vector<uint64> gehennasFlamewakers;
            vector<uint64> lucifronFlamewakers;
            vector<uint64> sulfuronFlamewakers;
            uint8 _deadBossCount;
            uint8 _ragnarosAddDeaths;
            uint64 _runesGUID[10] = { 0 };
            MoltenCoreRuneStatus _runesStatus[8] = { RUNE_UNUSED };
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_molten_core_InstanceMapScript(map);
        }
};

class go_molten_core_rune : public GameObjectScript
{
public:
    go_molten_core_rune() : GameObjectScript("go_molten_core_rune") { }

    bool OnGossipHello(Player* player, GameObject* go) override
    {
        instance_molten_core::instance_molten_core_InstanceMapScript* instanceScript = CAST_AI(instance_molten_core::instance_molten_core_InstanceMapScript, go->GetInstanceScript());
        if (instanceScript)
        {
            switch (go->GetEntry())
            {
            case GO_RUNE_SULFURON:                                    //Sulfuron
                if (instanceScript->UseRune(BOSS_SULFURON_HARBINGER, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_SULFURON, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            case GO_RUNE_BARON:                                    //Geddon
                if (instanceScript->UseRune(BOSS_BARON_GEDDON, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_BARON, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            case GO_RUNE_SHAZZRAH:                                    //Shazzrah
                if (instanceScript->UseRune(BOSS_SHAZZRAH, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_SHAZZRAH, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            case GO_RUNE_GOLEMAGG:                                    //Golemagg
                if (instanceScript->UseRune(BOSS_GOLEMAGG_THE_INCINERATOR, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_GOLEMAGG, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            case GO_RUNE_GARR:                                    //Garr
                if (instanceScript->UseRune(BOSS_GARR, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_GARR, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            case GO_RUNE_MAGMADAR:                                    //Magmadar
                if (instanceScript->UseRune(BOSS_MAGMADAR, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_MAGMADAR, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            case GO_RUNE_GEHENNAS:                                    //Gehennas
                if (instanceScript->UseRune(BOSS_GEHENNAS, go))
                {
                    if (GameObject* circle = go->FindNearestGameObject(GO_CIRCLE_GEHENNAS, 20.0f))
                        circle->Delete();
                    return false;
                }
                break;
            }
        }
        return true;
    }
};

class npc_firewalker : public CreatureScript
{
    public:
        npc_firewalker() : CreatureScript("npc_firewalker") { }

        struct npc_firewalkerAI : public ScriptedAI
        {
            npc_firewalkerAI(Creature* creature) : ScriptedAI(creature)
            {
            }

            void Reset()
            {
                events.Reset();
            }

            void JustDied(Unit*)
            {
                events.Reset();
            }

            void EnterCombat(Unit* victim)
            {
                ScriptedAI::EnterCombat(victim);
                events.ScheduleEvent(EVENT_INCITE_FLAMES, 8000);
                events.ScheduleEvent(EVENT_FIRE_BLOSSOM_CHECK, 12000);
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INCITE_FLAMES:
                            DoCast(SPELL_INCITE_FLAMES);
                            events.ScheduleEvent(EVENT_INCITE_FLAMES, urand(10000, 15000));
                            break;
                        case EVENT_FIRE_BLOSSOM_CHECK: 
                            DoCast(me, SPELL_ROOT); 
                            events.ScheduleEvent(EVENT_FIRE_BLOSSOM_CHECK, urand(12000, 15000));
                            events.ScheduleEvent(EVENT_CLEAR_ROOT, 6000);  
                            events.ScheduleEvent(EVENT_FIRE_BLOSSOM, 1000); // Fireball spell
                            break;
                        case EVENT_FIRE_BLOSSOM:
                            if(me->HasAura(SPELL_ROOT))
                               if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true, 0))
                               {
                                   DoCast(target, SPELL_FIRE_BLOSSOM);
                                   events.ScheduleEvent(EVENT_FIRE_BLOSSOM, 1000);
                               }
                           break;
                        case EVENT_CLEAR_ROOT:
                            me->RemoveAura(SPELL_ROOT); 
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap events;

        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_firewalkerAI(creature);
        }
};

void AddSC_instance_molten_core()
{
    new npc_firewalker();
    new instance_molten_core();
    new go_molten_core_rune();
}
