/*
 * @Author: fepo_h
 * @Date: 2022-12-16 15:37:12
 * @LastEditors: fepo_h
 * @LastEditTime: 2022-12-17 13:56:39
 * @FilePath: /fepoh/workspace/tests/test_sqlite/src/Sqlite.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by FepoH Fepo_H@163.com, All Rights Reserved. 
 * @version: V1.0.0
 * @Mailbox: Fepo_H@163.com
 * @Descripttion: 
 */
#include "Sqlite.h"

#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <iostream>

#define Log(str) \
    std::cout << str << std::endl;

Sqlite::Sqlite(sqlite3* db):m_db(db){
    //m_lastUsedTime = time(0);
}

Sqlite::~Sqlite(){
    close();
}

Sqlite::ptr Sqlite::Create(const std::string& db_name,int flags,char* module){
    sqlite3* db;
    if(sqlite3_open_v2(db_name.c_str(),&db,flags,module)){
        Log("open sqlite db error");
        return nullptr;
    }
    Sqlite::ptr sql(new Sqlite(db));
    return sql;
}

Sqlite::ptr Sqlite::Create(sqlite3* db){
    if(db == nullptr){
        return nullptr;
    }
    Sqlite::ptr d(new Sqlite(db));
    return d;
}

int Sqlite::getErrCode(){
    return sqlite3_errcode(m_db);
}

std::string Sqlite::getErrStr(){
    return sqlite3_errmsg(m_db);
}

int Sqlite::close(){
    int rt = SQLITE_OK;
    if(m_db){
        rt = sqlite3_close(m_db);
        if(rt == SQLITE_OK){
            m_db = nullptr;
        }
    }
    return rt;
}

SqliteStmt::ptr Sqlite::prepare(const std::string& pre_str){
    return SqliteStmt::Create(shared_from_this(),pre_str);
}

int Sqlite::execute(char* format,va_list ap){
    std::shared_ptr<char> sql(sqlite3_vmprintf(format,ap),sqlite3_free);
    return sqlite3_exec(m_db,sql.get(),NULL,NULL,NULL);
}

int Sqlite::execute(char* format,...){
    va_list ap;
    va_start(ap,format);
    int rt = execute(format,ap);
    va_end(ap);
    return rt;
}

int Sqlite::execute(const std::string& query){
    return sqlite3_exec(m_db,query.c_str(),NULL,NULL,NULL);
}

SqliteData::ptr Sqlite::query(char* format,...){
    va_list ap;
    va_start(ap,format);
    SqliteData::ptr sdata = query(format,ap);
    va_end(ap);
    return sdata;
}

SqliteData::ptr Sqlite::query(char* format,va_list vl){
    std::shared_ptr<char> sql(sqlite3_vmprintf(format,vl),sqlite3_free);
    SqliteStmt::ptr sstmt = SqliteStmt::Create(shared_from_this(),sql.get());
    if(!sstmt){
        return nullptr;
    }
    return sstmt->query();
}

SqliteData::ptr Sqlite::query(const std::string& qu){
    SqliteStmt::ptr sstmt = SqliteStmt::Create(shared_from_this(),qu);
    if(!sstmt){
        return nullptr;
    }
    return sstmt->query();
}

SqliteTransaction::ptr Sqlite::openTransaction(bool auto_commit ,int begin_type){
    SqliteTransaction::ptr tran =  SqliteTransaction::Create(shared_from_this(),auto_commit,begin_type);
    if(tran){
        return tran;
    }
    return nullptr;
}


/*
   SqliteStmt
*/
SqliteStmt::~SqliteStmt(){
    int rt = finish();
    if(rt){
        Log("~stmt finish error");
    }
}

int SqliteStmt::bindNull(int index){
    return sqlite3_bind_null(m_stmt,index);
}

int SqliteStmt::bindInt(int index,int val){
    return sqlite3_bind_int(m_stmt,index,val);
}

int SqliteStmt::bindDouble(int index,double val){
    return sqlite3_bind_double(m_stmt,index,val);
}

int SqliteStmt::bindInt64(int index,int64_t val){
    return sqlite3_bind_int64(m_stmt,index,val);
}

int SqliteStmt::bindStr(int index,const std::string& val,Type type){
    return bindStr(index,val.c_str(),type);
}

int SqliteStmt::bindStr(int index,const char* val,Type type){
    return sqlite3_bind_text(m_stmt,index,val,strlen(val)
        ,(type == COPY ? SQLITE_TRANSIENT : SQLITE_STATIC));
}

int SqliteStmt::bindBlob(int index,const char* val,int size,Type type){
    return sqlite3_bind_text(m_stmt,index,val,size
        ,(type == COPY ? SQLITE_TRANSIENT : SQLITE_STATIC));
}


SqliteStmt::ptr SqliteStmt::Create(Sqlite::ptr db,const std::string& pre_str){
    return SqliteStmt::Create(db,pre_str.c_str());
}

SqliteStmt::ptr SqliteStmt::Create(Sqlite::ptr db,const char* pre_str){
    sqlite3_stmt* stmt;
    int rt = sqlite3_prepare_v2(db->getDB(),pre_str,strlen(pre_str),&stmt,nullptr);
    if(rt){
        Log("stmt create error");
        return nullptr;
    }
    SqliteStmt::ptr sstmt(new SqliteStmt(db,stmt));
    return sstmt;
}

int SqliteStmt::step(){
    return sqlite3_step(m_stmt);
}

int SqliteStmt::finish(){
    int rt = SQLITE_OK;
    if(m_stmt){
        rt = sqlite3_finalize(m_stmt);
        if(rt == SQLITE_OK){
            m_stmt = nullptr;
        }
    }
    return rt;
}

int SqliteStmt::reset(){
    return sqlite3_reset(m_stmt);
}

int SqliteStmt::execute(){
    int rt = step();
    if(rt == SQLITE_DONE){
        rt = SQLITE_OK;
    }
    return rt;
}

//TODO
SqliteData::ptr SqliteStmt::query(){
    SqliteData::ptr sdata = SqliteData::Create(shared_from_this());
    return sdata;
}

int SqliteStmt::prepare(const std::string& pre_str){
    int rt = finish();
    if(rt){
        Log("stmt finish error");
        return rt;
    }
    rt = sqlite3_prepare_v2(m_db->getDB(),pre_str.c_str(),pre_str.size(),&m_stmt,nullptr);
    if(!rt){
        Log("stmt create error");
        return rt;
    }
    return rt;
}

int SqliteStmt::getErrCode(){
    return m_db->getErrCode();   
}

std::string SqliteStmt::getErrStr(){
    return m_db->getErrStr();
}

SqliteStmt::SqliteStmt(Sqlite::ptr sql):m_db(sql){
    m_stmt = nullptr;
}

SqliteStmt::SqliteStmt(Sqlite::ptr sql,sqlite3_stmt* stmt)
    :m_db(sql),m_stmt(stmt){
}

/*
    SqliteData
*/
SqliteData::ptr SqliteData::Create(SqliteStmt::ptr stmt){
    SqliteData::ptr sdata(new SqliteData(stmt));
    return sdata;
}

int SqliteData::getDataCount(){
    return sqlite3_data_count(m_stmt->getStmt());
}

int SqliteData::getColCount(){
    return sqlite3_column_count(m_stmt->getStmt());
}

int SqliteData::getColBytes(int index){
    return sqlite3_column_bytes(m_stmt->getStmt(),index);
}

int SqliteData::getColType(int index){
    return sqlite3_column_type(m_stmt->getStmt(),index);
}

std::string SqliteData::getColName(int index){
    const char* name = sqlite3_column_name(m_stmt->getStmt(),index);
    if(name){
        return name;
    }
    return "";
}

int SqliteData::getColInt(int index){
    return sqlite3_column_int(m_stmt->getStmt(),index);
}

double SqliteData::getColDouble(int index){
    return sqlite3_column_double(m_stmt->getStmt(),index);
}
std::string SqliteData::getColStr(int index){
    const char* str = (const char*)sqlite3_column_text(m_stmt->getStmt(),index);
    //std::cout << "+++"<<blob << "+++"<<std::endl;
    //std::cout <<"++" << getColBytes(index) << "++" << std::endl;
    if(str){
        return str;
    }
    return "";
}

std::string SqliteData::getColBlob(int index){
    const char* blob = (const char*)sqlite3_column_blob(m_stmt->getStmt(),index);
    if(blob){
        return blob;
    }
    return "";
}


SqliteData::SqliteData(SqliteStmt::ptr stmt)
    :m_stmt(stmt){
    
}

bool SqliteData::query(){
    int rt = m_stmt->step();
    if(m_firstQuery){
        m_errCode = m_stmt->getErrCode();
        m_errStr = m_stmt->getErrStr();
        m_firstQuery = false;
    }
    return rt == SQLITE_ROW;
}

bool SqliteData::step(){
    int rt = m_stmt->step();
    if(rt != SQLITE_ROW){
        return false;
    }
    return true;
}

/*
    SqliteTransaction
*/

SqliteTransaction::ptr SqliteTransaction::Create(Sqlite::ptr db,bool auto_commit,int begin_type){
    SqliteTransaction::ptr stran(new SqliteTransaction(db,auto_commit,begin_type));
    if(stran){
        return stran;
    }
    return nullptr;
}

SqliteTransaction::~SqliteTransaction(){
    if(m_status == 1){
        if(m_autoCommit){
            commit();
        }else{
            rollback();
        }
    }
}

bool SqliteTransaction::begin(){
    if(m_status == 0){
        char* sql = "BEGIN"; 
        switch(m_type){
            case IMMEDIATE:
                sql = "BEGIN IMMEDIATE";
                break;
            case EXCLUSIVE:
                sql = "BEGIN EXCLUSIVE";
                break;
            default:
                break;
        }
        int rt = m_db->execute("%s",sql);
        if(rt == SQLITE_OK){
            m_status = 1;
        }
        return rt == SQLITE_OK;
    }
    return false;;
}
bool SqliteTransaction::rollback(){
    if(m_status == 1){
        std::string sql("ROLLBACK");
        int rt = m_db->execute(sql);
        if(rt == SQLITE_OK){
            m_status = 2;
        }
        return rt == SQLITE_OK;
    }
    return false;
}
bool SqliteTransaction::commit(){
    if(m_status == 1){
        std::string sql("COMMIT");
        int rt = m_db->execute(sql);
        if(rt == SQLITE_OK){
            m_status = 2;
        }
        return rt == SQLITE_OK;
    }
    return false;
}

SqliteTransaction::SqliteTransaction(Sqlite::ptr db,bool auto_commit,int begin_type)
    :m_status(0),m_autoCommit(auto_commit),m_db(db){
    m_type = (BeginType)begin_type;
}
