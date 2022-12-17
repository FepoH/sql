/*
 * @Author: fepo_h
 * @Date: 2022-12-16 15:26:30
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-12-17 13:25:21
 * @FilePath: /fepoh/workspace/tests/test_sqlite/test.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */

#include <stdarg.h>
#include <string>
#include <string.h>
#include <iostream>
#include "Sqlite.h"

#define LOG(str)    \
    std::cout << str <<std::endl;

int main(){
const std::string dbname = "test.db";
    auto db = Sqlite::Create(dbname, Sqlite::READWRITE);
    if(!db) {
        LOG("not exist");
        db = Sqlite::Create(dbname
                , Sqlite::READWRITE | Sqlite::CREATE);
        if(!db) {
            LOG("create error");
            return 0;
        }

#define XX(...) #__VA_ARGS__
        int rt = db->execute(
XX(create table user (
        id integer primary key autoincrement,
        name varchar(50) not null default "",
        age int not null default 0,
        create_time datetime
        )));
#undef XX

        if(rt != SQLITE_OK) {
            LOG("create error");
            return 0;
        }
    }

    // std::string create = "cerate table user ("
    //             "id int,"
    //             "name varchar(255),"
    //             "age int not null default 0)";
    // db->execute(create);

    for(int i = 0; i < 10; ++i) {
        if(db->execute("insert into user(name, age) values(\"name_%d\",%d)", i, i)
                != SQLITE_OK) {
            LOG(db->getErrCode());
            LOG(db->getErrStr());
            LOG("insert error");
        }
    }

    SqliteStmt::ptr stmt = SqliteStmt::Create(db,
                "insert into user(name, age, create_time) values(?, ?, ?)");
    if(!stmt) {
        LOG("prepare error");
        return 0;
    }

    int64_t now = time(0);
    for(int i = 0; i < 10; ++i) {
        stmt->bindStr(1, "stmt_" + std::to_string(i));
        stmt->bindInt(2, i);
        stmt->bindInt(3, now + rand() % 100);
        //stmt->bind(3, Time2Str(now + rand() % 100));
        //stmt->bind(3, "stmt_" + std::to_string(i + 1));
        //stmt->bind(4, i + 1);

        if(stmt->execute() != SQLITE_OK) {
            LOG("stmt error");
        }
        stmt->reset();
    }

    SqliteStmt::ptr query = SqliteStmt::Create(db,
            "select * from user");
    if(!query) {
        return 0;
    }
    auto ds = query->query();
    if(!ds) {
        return 0;
    }

    // while(true) {
    //     int rt = ds->step();
    //     if(rt != SQLITE_ROW){
    //         break;
    //     }
    //     //SYLAR_LOG_INFO(g_logger) << "query ";
    // };


    auto dd = (db->query("select * from user"));
    while(dd->step()) {
        std::cout << "ds.data_count=" << dd->getColCount()
            << " ds.column_count=" << dd->getColCount()
            << " 0=" << dd->getColInt(0) << " 1=" << dd->getColStr(1)
            << " 2=" << dd->getColStr(2)
            << " 3=" << dd->getColStr(3) << std::endl;
        
    }

    //test_batch(db);
    return 0;


    // std::string create = "cerate table user_info ("
    //             "id int AUTOINCREMENT,"
    //             "name varchar(255))";
    // db->execute(create);
    // // std::string insert = 
    // for(int i =0;i<10;++i){
    //     int rt = db->execute("insert into user_info (name) values (%s)","name_"+std::to_string(i));
    //     if(rt){
    //         LOG(db->getErrCode());
    //         LOG(db->getErrStr());
    //     }
    // }
    

    return 0;
}