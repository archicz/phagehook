#ifndef GARRYSMOD_LUA_LUABASE_H
#define GARRYSMOD_LUA_LUABASE_H

#include <stddef.h>

#include "Types.h"
#include "UserData.h"
#include "SourceCompat.h"

struct lua_State;

struct clr_t
{
	unsigned char r, g, b, a;
};

namespace GarrysMod
{
    namespace Lua
    {
        typedef int ( *CFunc )( lua_State* L );

        //
        // Use this to communicate between C and Lua
        //
        class ILuaBase
        {
            public:

                // You shouldn't need to use this struct
                // Instead, use the UserType functions
                struct UserData
                {
                    void*         data;
                    unsigned char type;
                };

                // Returns the amount of values on the stack
                virtual int         Top( void ) = 0;

                // Pushes a copy of the value at iStackPos to the top of the stack
                virtual void        Push( int iStackPos ) = 0;

                // Pops iAmt values from the top of the stack
                virtual void        Pop( int iAmt = 1 ) = 0;

                // Pushes table[key] on to the stack
                // table = value at iStackPos
                // key   = value at top of the stack
                virtual void        GetTable( int iStackPos ) = 0;

                // Pushes table[key] on to the stack
                // table = value at iStackPos
                // key   = strName
                virtual void        GetField( int iStackPos, const char* strName ) = 0;

                // Sets table[key] to the value at the top of the stack
                // table = value at iStackPos
                // key   = strName
                // Pops the value from the stack
                virtual void        SetField( int iStackPos, const char* strName ) = 0;

                // Creates a new table and pushes it to the top of the stack
                virtual void        CreateTable() = 0;

                // Sets table[key] to the value at the top of the stack
                // table = value at iStackPos
                // key   = value 2nd to the top of the stack
                // Pops the key and the value from the stack
                virtual void        SetTable( int iStackPos ) = 0;

                // Sets the metatable for the value at iStackPos to the value at the top of the stack
                // Pops the value off of the top of the stack
                virtual void        SetMetaTable( int iStackPos ) = 0;
                
                // Pushes the metatable of the value at iStackPos on to the top of the stack
                // Upon failure, returns false and does not push anything
                virtual bool        GetMetaTable( int i ) = 0;

                // Calls a function
                // To use it: Push the function on to the stack followed by each argument
                // Pops the function and arguments from the stack, leaves iResults values on the stack
                // If this function errors, any local C values will not have their destructors called!
                virtual void        Call( int iArgs, int iResults ) = 0;

                // Similar to Call
                // See: lua_pcall( lua_State*, int, int, int )
                virtual int         PCall( int iArgs, int iResults, int iErrorFunc ) = 0;

                // Returns true if the values at iA and iB are equal
                virtual int         Equal( int iA, int iB ) = 0;

                // Returns true if the value at iA and iB are equal
                // Does not invoke metamethods
                virtual int         RawEqual( int iA, int iB ) = 0;

                // Moves the value at the top of the stack in to iStackPos
                // Any elements above iStackPos are shifted upwards
                virtual void        Insert( int iStackPos ) = 0;
                
                // Removes the value at iStackPos from the stack
                // Any elements above iStackPos are shifted downwards
                virtual void        Remove( int iStackPos ) = 0;

                // Allows you to iterate tables similar to pairs(...)
                // See: lua_next( lua_State*, int );
                virtual int         Next( int iStackPos ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
            private:
#endif
                // Deprecated: Use the UserType functions instead of this
                virtual void*       NewUserdata( unsigned int iSize ) = 0;

            public:
                // Throws an error and ceases execution of the function
                // If this function is called, any local C values will not have their destructors called!
                virtual void        ThrowError( const char* strError ) = 0;

                // Checks that the type of the value at iStackPos is iType
                // Throws and error and ceases execution of the function otherwise
                // If this function errors, any local C values will not have their destructors called!
                virtual void        CheckType( int iStackPos, int iType ) = 0;

                // Throws a pretty error message about the given argument
                // If this function is called, any local C values will not have their destructors called!
                virtual void        ArgError( int iArgNum, const char* strMessage ) = 0;

                // Pushes table[key] on to the stack
                // table = value at iStackPos
                // key   = value at top of the stack
                // Does not invoke metamethods
                virtual void        RawGet( int iStackPos ) = 0;

                // Sets table[key] to the value at the top of the stack
                // table = value at iStackPos
                // key   = value 2nd to the top of the stack
                // Pops the key and the value from the stack
                // Does not invoke metamethods
                virtual void        RawSet( int iStackPos ) = 0;

                // Returns the string at iStackPos. iOutLen is set to the length of the string if it is not NULL
                // If the value at iStackPos is a number, it will be converted in to a string
                // Returns NULL upon failure
                virtual const char* GetString( int iStackPos = -1, unsigned int* iOutLen = NULL ) = 0;

                // Returns the number at iStackPos
                // Returns 0 upon failure
                virtual double      GetNumber( int iStackPos = -1 ) = 0;

                // Returns the boolean at iStackPos
                // Returns false upon failure
                virtual bool        GetBool( int iStackPos = -1 ) = 0;

                // Returns the C-Function at iStackPos
                // returns NULL upon failure
                virtual CFunc       GetCFunction( int iStackPos = -1 ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
            private:
#endif
                // Deprecated: You should probably be using the UserType functions instead of this
                virtual void*   GetUserdata( int iStackPos = -1 ) = 0;

            public:
                // Pushes a nil value on to the stack
                virtual void        PushNil() = 0;

                // Pushes the given string on to the stack
                // If iLen is 0, strlen will be used to determine the string's length
                virtual void        PushString( const char* val, unsigned int iLen = 0 ) = 0;

                // Pushes the given double on to the stack
                virtual void        PushNumber( double val ) = 0;

                // Pushes the given bobolean on to the stack
                virtual void        PushBool( bool val ) = 0;

                // Pushes the given C-Function on to the stack
                virtual void        PushCFunction( CFunc val ) = 0;

                // Pushes the given C-Function on to the stack with upvalues
                // See: GetUpvalueIndex()
                virtual void        PushCClosure( CFunc val, int iVars ) = 0;

                // Pushes the given pointer on to the stack as light-userdata
                virtual void        PushUserdata( void* ) = 0;

                // Allows for values to be stored by reference for later use
                // Make sure you call ReferenceFree when you are done with a reference
                virtual int         ReferenceCreate() = 0;
                virtual void        ReferenceFree( int i ) = 0;
                virtual void        ReferencePush( int i ) = 0;

                // Push a special value onto the top of the stack (see SPECIAL_* enums)
                virtual void        PushSpecial( int iType ) = 0;

                // Returns true if the value at iStackPos is of type iType
                // See: Types.h
                virtual bool        IsType( int iStackPos, int iType ) = 0;

                // Returns the type of the value at iStackPos
                // See: Types.h
                virtual int         GetType( int iStackPos ) = 0;

                // Returns the name associated with the given type ID
                // See: Types.h
                // Note: GetTypeName does not work with user-created types
                virtual const char* GetTypeName( int iType ) = 0;

#ifndef GMOD_ALLOW_DEPRECATED
            private:
#endif
                // Deprecated: Use CreateMetaTable
                virtual void        CreateMetaTableType( const char* strName, int iType ) = 0;

            public:
                // Like Get* but throws errors and returns if they're not of the expected type
                // If these functions error, any local C values will not have their destructors called!
                virtual const char* CheckString( int iStackPos = -1 ) = 0;
                virtual double      CheckNumber( int iStackPos = -1 ) = 0;
                
                // Returns the length of the object at iStackPos
                // Works for: strings, tables, userdata
                virtual int         ObjLen( int iStackPos = -1 ) = 0;

                // Returns the angle at iStackPos
                virtual const QAngle& GetAngle( int iStackPos = -1 ) = 0;
                
                // Returns the vector at iStackPos
                virtual const Vector& GetVector( int iStackPos = -1 ) = 0;

                // Pushes the given angle to the top of the stack
                virtual void        PushAngle( const QAngle& val ) = 0;

                // Pushes the given vector to the top of the stack
                virtual void        PushVector( const Vector& val ) = 0;

                // Sets the lua_State to be used by the ILuaBase implementation
                // You don't need to use this if you use the LUA_FUNCTION macro
                virtual void        SetState( lua_State *L ) = 0;

                // Pushes the metatable associated with the given type name
                // Returns the type ID to use for this type
                // If the type doesn't currently exist, it will be created
                virtual int         CreateMetaTable( const char* strName ) = 0;

                // Pushes the metatable associated with the given type
                virtual bool        PushMetaTable( int iType ) = 0;

                // Creates a new UserData of type iType that references the given data
                virtual void        PushUserType( void* data, int iType ) = 0;
                
                // Sets the data pointer of the UserType at iStackPos
                // You can use this to invalidate a UserType by passing NULL
                virtual void        SetUserType( int iStackPos, void* data ) = 0;

				virtual void Init(void *, bool) = 0;
				virtual void Shutdown(void) = 0;
				virtual void Cycle(void) = 0;
				virtual void Global(void) = 0;
				virtual void* GetObject(int) = 0;
				virtual void PushLuaObject(void *) = 0;
				virtual void PushLuaFunction(CFunc) = 0;
				virtual void LuaError(char const*, int) = 0;
				virtual void TypeError(char const*, int) = 0;
				virtual void CallInternal(int, int) = 0;
				virtual void CallInternalNoReturns(int) = 0;
				virtual void CallInternalGetBool(int) = 0;
				virtual void CallInternalGetString(int) = 0;
				virtual void CallInternalGet(int, void *) = 0;
				virtual void NewGlobalTable(char const*) = 0;
				virtual void NewTemporaryObject(void) = 0;
				virtual void isUserData(int) = 0;
				virtual void GetMetaTableObject(char const*, int) = 0;
				virtual void GetMetaTableObject(int) = 0;
				virtual void GetReturn(int) = 0;
				virtual void IsServer(void) = 0;
				virtual void IsClient(void) = 0;
				virtual void IsDedicatedServer(void) = 0;
				virtual void DestroyObject(void *) = 0;
				virtual void CreateObject(void) = 0;
				virtual void SetMember(void *, void *, void *) = 0;
				virtual void GetNewTable(void) = 0;
				virtual void SetMember(void *, float) = 0;
				virtual void SetMember(void *, float, void *) = 0;
				virtual void SetMember(void *, char const*) = 0;
				virtual void SetMember(void *, char const*, void *) = 0;
				virtual void SetIsServer(bool) = 0;
				virtual void PushLong(long) = 0;
				virtual void GetFlags(int) = 0;
				virtual void FindOnObjectsMetaTable(int, int) = 0;
				virtual void FindObjectOnTable(int, int) = 0;
				virtual void SetMemberFast(void *, int, int) = 0;
				virtual void RunString(char const* filename, char const* path, char const* stringtoun, bool run = true, bool showerrors = true) = 0;
				virtual void IsEqual(void *, void *) = 0;
				virtual void Error(char const*) = 0;
				virtual void GetStringOrError(int) = 0;
				virtual void RunLuaModule(char const*) = 0;
				virtual void FindAndRunScript(const char* path, bool run = true, bool showerrors = true, const char* type = "!CLIENT") = 0;
				virtual void SetPathID(char const*) = 0;
				virtual void GetPathID(void) = 0;
				virtual void ErrorNoHalt(char const*, ...) = 0;
				virtual void Msg(char const*, ...) = 0;
				virtual void PushPath(char const*) = 0;
				virtual void PopPath(void) = 0;
				virtual void GetPath(void) = 0;
				virtual void GetColor(int) = 0;
				virtual void PushColor(clr_t) = 0;
				virtual int GetStack(int, void *) = 0;
				virtual void GetInfo(char const*, void *) = 0;
				virtual void GetLocal(void *, int) = 0;
				virtual void GetUpvalue(int, int) = 0;
				virtual void RunStringEx(void*, char const* filename, char const* path, char const* torun, bool run, bool showerrors, bool idk, bool idk2) = 0;
				virtual void GetDataString(int, void **) = 0;
				virtual void ErrorFromLua(char const*, ...) = 0;
				virtual void GetCurrentLocation(void) = 0;
				virtual void MsgColour(clr_t const&, char const*, ...) = 0;
				virtual void GetCurrentFile(std::string &) = 0;
				virtual void CompileString(int &, std::string const&) = 0;
				virtual void CallFunctionProtected(int, int, bool) = 0;
				virtual void Require(char const*) = 0;
				virtual void GetActualTypeName(int) = 0;
				virtual void PreCreateTable(int, int) = 0;
				virtual void PushPooledString(int) = 0;
				virtual void GetPooledString(int) = 0;
				virtual void AddThreadedCall(void *) = 0;

                // Returns the data of the UserType at iStackPos if it is of the given type
                template <class T>
                T* GetUserType( int iStackPos, int iType )
                {
                    UserData* ud = (UserData*) GetUserdata( iStackPos );

                    if ( ud == NULL || ud->data == NULL || ud->type != iType )
                        return NULL;

                    return reinterpret_cast<T*>( ud->data );
                }
        };

        // For use with ILuaBase::PushSpecial
        enum
        {
            SPECIAL_GLOB,       // Global table
            SPECIAL_ENV,        // Environment table
            SPECIAL_REG,        // Registry table
        };
    }
}

#endif
