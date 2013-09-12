// -------------------------------------------------------------------
//
// db_list.cc
//
// Copyright (c) 2011-2013 Basho Technologies, Inc. All Rights Reserved.
//
// This file is provided to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file
// except in compliance with the License.  You may obtain
// a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// -------------------------------------------------------------------

#include <syslog.h>
#include <algorithm>

#include "util/db_list.h"

namespace leveldb {

// using singleton model from comparator.cc
static port::OnceType once = LEVELDB_ONCE_INIT;
static DBListImpl * dblist=NULL;

static void InitModule()
{
    dblist=new DBListImpl;

    syslog(LOG_ERR,"DBListImpl::InitModule");
}   // InitModule


DBListImpl * DBList()
{
    port::InitOnce(&once, InitModule);
    return(dblist);

}   // DBList


void
DBListShutdown()
{
    // retrieve point to handle any initialization/shutdown races
    DBList();
    delete dblist;

    syslog(LOG_ERR,"DBListImpl::DBListShutdown");

    return;

}   // DBListShutdown



DBListImpl::DBListImpl()
{
}   // DBListImpl::DBListImpl


void
DBListImpl::AddDB(
    DBImpl * Dbase,
    bool IsInternal)
{
    if (IsInternal)
    {
        if (m_InternalDBs.insert(Dbase).second)
            syslog(LOG_ERR,"DBListImpl::AddDB succeeded");
        else
            syslog(LOG_ERR,"DBListImpl::AddDB failed");

    }   // if
    else
    {
        if (m_UserDBs.insert(Dbase).second)
            syslog(LOG_ERR,"DBListImpl::AddDB succeeded");
        else
            syslog(LOG_ERR,"DBListImpl::AddDB failed");
    }   // else

    return;

}   // DBListImpl::AddDB


void
DBListImpl::ReleaseDB(
    DBImpl * Dbase,
    bool IsInternal)
{
    db_set_t::iterator it;

    if (IsInternal)
    {
        it=m_InternalDBs.find(Dbase);
        if (m_InternalDBs.end()!=it)
        {
            m_InternalDBs.erase(it);
            syslog(LOG_ERR,"DBListImpl::ReleaseDB succeeded");
        }   // if
        else
            syslog(LOG_ERR,"DBListImpl::ReleaseDB failed");

    }   // if
    else
    {
        it=m_UserDBs.find(Dbase);
        if (m_UserDBs.end()!=it)
        {
            m_UserDBs.erase(it);
            syslog(LOG_ERR,"DBListImpl::ReleaseDB succeeded");
        }   // if
        else
            syslog(LOG_ERR,"DBListImpl::ReleaseDB failed");

    }   // else

    return;

}   // DBListImpl::ReleaseDB


size_t
DBListImpl::GetDBCount(
    bool IsInternal)
{
    size_t ret_val;

    if (IsInternal)
        ret_val=m_InternalDBs.size();
    else
        ret_val=m_UserDBs.size();

    return(ret_val);

}   // DBListImpl::GetDBCount


void
DBListImpl::ScanDBs(
    bool IsInternal,
    void (DBImpl::* Function)())
{
    if (IsInternal)
        for_each(m_InternalDBs.begin(), m_InternalDBs.end(), std::mem_fun(Function));
    else
        for_each(m_UserDBs.begin(), m_UserDBs.end(), std::mem_fun(Function));

    return;

}   // DBListImpl::AddDB




}  // namespace leveldb
