/*
 * @Author: fepo_h
 * @Date: 2022-12-16 15:28:28
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-12-17 13:34:32
 * @FilePath: /fepoh/workspace/tests/test_sqlite/src/Sqlite.h
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#pragma once

#include <sqlite3.h>
#include <memory>
#include <string>

class SqliteStmt;
class SqliteData;
class SqliteTransaction;

class Sqlite : public std::enable_shared_from_this<Sqlite>{
public:
    typedef std::shared_ptr<Sqlite> ptr;
    enum Flags{
        READONLY = SQLITE_READONLY,
        READWRITE = SQLITE_OPEN_READWRITE,
        CREATE = SQLITE_OPEN_CREATE
    };
    ~Sqlite();
    int close();        //关闭数据库
    
    std::shared_ptr<SqliteStmt> prepare(const std::string& ptr_str);

    int execute(char* format,...);
    int execute(char* format,va_list vl);
    int execute(const std::string& qu);

    std::shared_ptr<SqliteData> query(char* format,...);
    std::shared_ptr<SqliteData> query(char* format,va_list vl);
    std::shared_ptr<SqliteData> query(const std::string& qu);
    
    std::shared_ptr<SqliteTransaction> openTransaction(bool auto_commit = true
                                                    ,int begin_type = 0);

    int getErrCode();     //获取错误
    std::string getErrStr();    //获取错误描述
    sqlite3* getDB() const {return m_db;}
    uint64_t getLastUsedTime() const {return m_lastUsedTime;}
    void setLastUsedTime(uint64_t val) {m_lastUsedTime = val;}
public:
    static Sqlite::ptr Create(const std::string& db_name
        ,int flags = READWRITE | CREATE,char* module = nullptr);
    static Sqlite::ptr Create(sqlite3* db);
private:
    Sqlite(sqlite3* db);
private:
    sqlite3* m_db = nullptr;
    uint64_t m_lastUsedTime = 0;    //管理器设置
};

class SqliteData;

class SqliteStmt : public std::enable_shared_from_this<SqliteStmt>{
public:
    typedef std::shared_ptr<SqliteStmt> ptr;

    enum Type{
        COPY = 1,
        NO_COPY = 2
    };

    ~SqliteStmt();

    static SqliteStmt::ptr Create(Sqlite::ptr db,const std::string& pre_str);
    static SqliteStmt::ptr Create(Sqlite::ptr db,const char* pre_str);



    int bindNull(int index);
    int bindInt(int index,int val);
    int bindDouble(int index,double val);
    int bindInt64(int index,int64_t val);
    int bindStr(int index,const std::string& val,Type type = COPY);
    int bindStr(int index,const char* val,Type type = COPY);
    int bindBlob(int index,const char* val,int size,Type type = COPY);

    int step();
    
    int execute();
    std::shared_ptr<SqliteData> query();
    int prepare(const std::string& pre_str);
    int finish();
    int reset();

    int getErrCode();
    std::string getErrStr();

    Sqlite::ptr getDB() const {return m_db;}
    sqlite3_stmt* getStmt() const {return m_stmt;}
private:
    SqliteStmt(Sqlite::ptr sql);
    SqliteStmt(Sqlite::ptr sql,sqlite3_stmt* stmt);
private:
    Sqlite::ptr m_db;
    sqlite3_stmt* m_stmt;
};


class SqliteData{
public:
    typedef std::shared_ptr<SqliteData> ptr;

    ~SqliteData() = default;
    static SqliteData::ptr Create(SqliteStmt::ptr stm);

    int getDataCount();
    int getColCount();
    int getColBytes(int index);
    int getColType(int index);
    std::string getColName(int index);

    int getColInt(int index);
    double getColDouble(int index);
    std::string getColStr(int index);
    std::string getColBlob(int index);

    bool query();
    bool step();

private:
    SqliteData(SqliteStmt::ptr stmt);
private:
    int m_errCode = 0;
    std::string m_errStr = "";
    bool m_firstQuery = true;
    SqliteStmt::ptr m_stmt;
};

class SqliteTransaction{
public:
    typedef std::shared_ptr<SqliteTransaction> ptr;

    enum BeginType{
        DEFERRED = 1,
        IMMEDIATE = 2,
        EXCLUSIVE = 3
    };

    static SqliteTransaction::ptr Create(Sqlite::ptr db,bool auto_commit = true,int begin_type = 1);

    ~SqliteTransaction();

    bool begin();
    bool rollback();
    bool commit();

private:
    SqliteTransaction(Sqlite::ptr,bool auto_commit,int begin_type);

private:
    int m_status;
    BeginType m_type;
    bool m_autoCommit;
    Sqlite::ptr m_db;
};

class SqliteManager{

};