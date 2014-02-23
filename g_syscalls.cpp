// Copyright (C) 2001-2002 Raven Software.
//
#include "g_local.h"
//#include "bg_public.h"

// this file is only included when building a dll
// g_syscalls.asm is included instead when building a qvm

static intptr_t(*syscall)(intptr_t arg, ...) = (intptr_t(*)(intptr_t, ...)) - 1;


Q_EXPORT void dllEntry( intptr_t ( *syscallptr)( int arg,... ) ) {
	syscall = syscallptr;
}

// Henk 22/02/2014 -> So we use less code and have the same layout message everywhere.
void infoMsgToClients(int clientNum, const char *msg){ 
	trap_SendServerCommand(clientNum, va("print \"^3[Info] ^7%s.\n\"", msg));
}

int PASSFLOAT( float x ) {
	float	floatTemp;
	floatTemp = x;
	return *(int *)&floatTemp;
}

void	trap_Print( const char *text ) {
	syscall( G_PRINT, text );
}

void	trap_Error( const char *text ) {
	syscall( G_ERROR, text );
}

int		trap_Milliseconds( void ) {
	return syscall( G_MILLISECONDS ); 
}
int		trap_Argc( void ) {
	return syscall( G_ARGC );
}

void	trap_Argv( int n, char *buffer, int bufferLength ) {
	syscall( G_ARGV, n, buffer, bufferLength );
}

int		trap_FS_FOpenFile( const char *qpath, fileHandle_t *f, fsMode_t mode ) {
	return syscall( G_FS_FOPEN_FILE, qpath, f, mode );
}

void	trap_FS_Read( void *buffer, int len, fileHandle_t f ) {
	syscall( G_FS_READ, buffer, len, f );
}

void	trap_FS_Write( const void *buffer, int len, fileHandle_t f ) {
	syscall( G_FS_WRITE, buffer, len, f );
}

void	trap_FS_FCloseFile( fileHandle_t f ) {
	syscall( G_FS_FCLOSE_FILE, f );
}

int trap_FS_GetFileList(  const char *path, const char *extension, char *listbuf, int bufsize ) {
	return syscall( G_FS_GETFILELIST, path, extension, listbuf, bufsize );
}

void	trap_SendConsoleCommand( int exec_when, const char *text ) {
	syscall( G_SEND_CONSOLE_COMMAND, exec_when, text );
}

void	trap_Cvar_Register( vmCvar_t *cvar, const char *var_name, const char *value, int flags, float MinValue, float MaxValue ) {
	syscall( G_CVAR_REGISTER, cvar, var_name, value, flags, PASSFLOAT(MinValue), PASSFLOAT(MaxValue) );
}

void	trap_Cvar_Update( vmCvar_t *cvar ) {
	syscall( G_CVAR_UPDATE, cvar );
}

void trap_Cvar_Set( const char *var_name, const char *value ) {
	syscall( G_CVAR_SET, var_name, value );
}

int trap_Cvar_VariableIntegerValue( const char *var_name ) {
	return syscall( G_CVAR_VARIABLE_INTEGER_VALUE, var_name );
}

void trap_Cvar_VariableStringBuffer( const char *var_name, char *buffer, int bufsize ) {
	syscall( G_CVAR_VARIABLE_STRING_BUFFER, var_name, buffer, bufsize );
}


void trap_LocateGameData( gentity_t *gEnts, int numGEntities, int sizeofGEntity_t,
						 playerState_t *clients, int sizeofGClient ) {
	syscall( G_LOCATE_GAME_DATA, gEnts, numGEntities, sizeofGEntity_t, clients, sizeofGClient );
}

void trap_RMG_Init(int terrainID)
{
	syscall(G_RMG_INIT, terrainID);
}

void trap_DropClient( int clientNum, const char *reason ) {
	syscall( G_DROP_CLIENT, clientNum, reason );
}

void trap_SendServerCommand( int clientNum, const char *text ) {
	syscall( G_SEND_SERVER_COMMAND, clientNum, text );
}

void trap_SetConfigstring( int num, const char *string ) {
	syscall( G_SET_CONFIGSTRING, num, string );
}

void trap_GetConfigstring( int num, char *buffer, int bufferSize ) {
	syscall( G_GET_CONFIGSTRING, num, buffer, bufferSize );
}

void trap_GetUserinfo( int num, char *buffer, int bufferSize ) {
	syscall( G_GET_USERINFO, num, buffer, bufferSize );
}

void trap_SetUserinfo( int num, const char *buffer ) {
	syscall( G_SET_USERINFO, num, buffer );
}

void trap_GetServerinfo( char *buffer, int bufferSize ) {
	syscall( G_GET_SERVERINFO, buffer, bufferSize );
}

void trap_SetBrushModel( gentity_t *ent, const char *name ) {
	syscall( G_SET_BRUSH_MODEL, ent, name );
}

void trap_SetActiveSubBSP(int index)
{
	syscall( G_SET_ACTIVE_SUBBSP, index );
}

void trap_Trace( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	syscall( G_TRACE, results, start, mins, maxs, end, passEntityNum, contentmask, 0, 10 );
}

void trap_TraceCapsule( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) {
	syscall( G_TRACECAPSULE, results, start, mins, maxs, end, passEntityNum, contentmask, 0, 10 );
}

int trap_PointContents( const vec3_t point, int passEntityNum ) {
	return syscall( G_POINT_CONTENTS, point, passEntityNum );
}


bool trap_InPVS( const vec3_t p1, const vec3_t p2 ) {
	return syscall( G_IN_PVS, p1, p2 );
}

bool trap_InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 ) {
	return syscall( G_IN_PVS_IGNORE_PORTALS, p1, p2 );
}

void trap_AdjustAreaPortalState( gentity_t *ent, bool open ) {
	syscall( G_ADJUST_AREA_PORTAL_STATE, ent, open );
}

bool trap_AreasConnected( int area1, int area2 ) {
	return syscall( G_AREAS_CONNECTED, area1, area2 );
}

void trap_LinkEntity( gentity_t *ent ) {
	syscall( G_LINKENTITY, ent );
}

void trap_UnlinkEntity( gentity_t *ent ) {
	syscall( G_UNLINKENTITY, ent );
}

int trap_EntitiesInBox( const vec3_t mins, const vec3_t maxs, int *list, int maxcount ) {
	return syscall( G_ENTITIES_IN_BOX, mins, maxs, list, maxcount );
}

bool trap_EntityContact( const vec3_t mins, const vec3_t maxs, const gentity_t *ent ) {
	return syscall( G_ENTITY_CONTACT, mins, maxs, ent );
}

bool trap_EntityContactCapsule( const vec3_t mins, const vec3_t maxs, const gentity_t *ent ) {
	return syscall( G_ENTITY_CONTACTCAPSULE, mins, maxs, ent );
}

int trap_BotAllocateClient( void ) {
	return syscall( G_BOT_ALLOCATE_CLIENT );
}

void trap_BotFreeClient( int clientNum ) {
	syscall( G_BOT_FREE_CLIENT, clientNum );
}

void trap_GetUsercmd( int clientNum, usercmd_t *cmd ) {
	syscall( G_GET_USERCMD, clientNum, cmd );
}

bool trap_GetEntityToken( char *buffer, int bufferSize ) {
	return syscall( G_GET_ENTITY_TOKEN, buffer, bufferSize );
}

void *trap_BotGetMemoryGame(int size)
{
	void *ptr;

	ptr = (void *)syscall( G_BOT_GET_MEMORY, size );

	return ptr;
}

void trap_BotFreeMemoryGame(void *ptr)
{
	syscall( G_BOT_FREE_MEMORY, ptr);
}

void trap_SnapVector( float *v ) {
	syscall( G_SNAPVECTOR, v );
}

int trap_G2API_InitGhoul2Model(void **ghoul2Ptr, const char *fileName, int modelIndex, qhandle_t customSkin,
						  qhandle_t customShader, int modelFlags, int lodBias)
{
	return syscall(G_G2_INITGHOUL2MODEL, ghoul2Ptr, fileName, modelIndex, customSkin, customShader, modelFlags, lodBias);
}

void trap_G2API_CleanGhoul2Models ( void **ghoul2Ptr )
{
	syscall(G_G2_CLEANMODELS, ghoul2Ptr);
}

void trap_G2API_CollisionDetect ( 
	CollisionRecord_t *collRecMap, 
	void* ghoul2, 
	const vec3_t angles, 
	const vec3_t position,
	int frameNumber, 
	int entNum, 
	vec3_t rayStart, 
	vec3_t rayEnd, 
	vec3_t scale, 
	int traceFlags, 
	int useLod
	)
{
	syscall ( G_G2_COLLISIONDETECT, collRecMap, ghoul2, angles, position, frameNumber, entNum, rayStart, rayEnd, scale, traceFlags, useLod );
}

qhandle_t trap_G2API_RegisterSkin ( const char *skinName, int numPairs, const char *skinPairs)
{
	return syscall(G_G2_REGISTERSKIN, skinName, numPairs, skinPairs );
}

bool trap_G2API_SetSkin ( void* ghoul2, int modelIndex, qhandle_t customSkin)
{
	return syscall(G_G2_SETSKIN, ghoul2, modelIndex, customSkin );
}

bool trap_G2API_GetAnimFileNameIndex ( void* ghoul2, qhandle_t modelIndex, const char* filename )
{
	return syscall(G_G2_GETANIMFILENAMEINDEX, ghoul2, modelIndex, filename );
}

bool trap_G2API_SetBoneAngles(void *ghoul2, int modelIndex, const char *boneName, const vec3_t angles, const int flags,
								const int up, const int right, const int forward, qhandle_t *modelList,
								int blendTime , int currentTime )
{
	return (syscall(G_G2_ANGLEOVERRIDE, ghoul2, modelIndex, boneName, angles, flags, up, right, forward, modelList, blendTime, currentTime));
}

bool trap_G2API_SetBoneAnim(void *ghoul2, const int modelIndex, const char *boneName, const int startFrame, const int endFrame,
							  const int flags, const float animSpeed, const int currentTime, const float setFrame , const int blendTime )
{
	return syscall(G_G2_PLAYANIM, ghoul2, modelIndex, boneName, startFrame, endFrame, flags, PASSFLOAT(animSpeed), currentTime, PASSFLOAT(setFrame), blendTime);
}

TGenericParser2 trap_GP_ParseFile(char *fileName, bool cleanFirst, bool writeable)
{
	return (TGenericParser2)syscall(G_GP_PARSE_FILE, fileName, cleanFirst, writeable);
}

void trap_GP_Delete(TGenericParser2 *GP2)
{
	syscall(G_GP_DELETE, GP2);
}

TGPGroup trap_GP_GetBaseParseGroup(TGenericParser2 GP2)
{
	return (TGPGroup)syscall(G_GP_GET_BASE_PARSE_GROUP, GP2);
}

// CGPGroup (void *) routines
bool trap_GPG_GetName(TGPGroup GPG, char *Value)
{
	return (bool)syscall(G_GPG_GET_NAME, GPG, Value);
}

TGPGroup trap_GPG_GetNext(TGPGroup GPG)
{
	return (TGPGroup)syscall(G_GPG_GET_NEXT, GPG);
}

TGPGroup trap_GPG_GetPairs(TGPGroup GPG)
{
	return (TGPGroup)syscall(G_GPG_GET_PAIRS, GPG);
}

TGPGroup trap_GPG_GetSubGroups(TGPGroup GPG)
{
	return (TGPGroup)syscall(G_GPG_GET_SUBGROUPS, GPG);
}

TGPValue trap_GPG_FindSubGroup(TGPGroup GPG, const char *name)
{
	return (TGPValue)syscall(G_GPG_FIND_SUBGROUP, GPG, name);
}

TGPValue trap_GPG_FindPair(TGPGroup GPG, const char *key)
{
	return (TGPValue)syscall(G_GPG_FIND_PAIR, GPG, key);
}

bool trap_GPG_FindPairValue(TGPGroup GPG, const char *key, const char *defaultVal, char *Value)
{
	return (bool)syscall(G_GPG_FIND_PAIRVALUE, GPG, key, defaultVal, Value);
}

bool trap_GPV_GetName(TGPValue GPV, char *Value)
{
	return (bool)syscall(G_GPV_GET_NAME, GPV, Value);
}

TGPValue trap_GPV_GetNext(TGPValue GPV)
{
	return (TGPValue)syscall(G_GPV_GET_NEXT, GPV);
}

bool trap_GPV_IsList(TGPValue GPV)
{
	return (bool)syscall(G_GPV_IS_LIST, GPV);
}

bool trap_GPV_GetTopValue(TGPValue GPV, char *Value)
{
	return (bool)syscall(G_GPV_GET_TOP_VALUE, GPV, Value);
}

TGPValue trap_GPV_GetList(TGPValue GPV)
{
	return (TGPValue)syscall(G_GPV_GET_LIST, GPV);
}

int	trap_CM_RegisterTerrain(const char *config)
{
	return syscall(G_CM_REGISTER_TERRAIN, config);
}

void trap_GetWorldBounds ( vec3_t mins, vec3_t maxs )
{
	syscall ( G_GET_WORLD_BOUNDS, mins, maxs );
}

void *trap_VM_LocalAlloc ( int size )
{
	return (void *)syscall ( G_VM_LOCALALLOC, size );
}

void *trap_VM_LocalAllocUnaligned ( int size )
{
	return (void *)syscall ( G_VM_LOCALALLOCUNALIGNED, size );
}

void *trap_VM_LocalTempAlloc( int size )
{
	return (void *)syscall ( G_VM_LOCALTEMPALLOC, size );
}

void trap_VM_LocalTempFree( int size )
{
	syscall ( G_VM_LOCALTEMPFREE, size );
}

const char *trap_VM_LocalStringAlloc ( const char *source )
{
	return (const char *)syscall ( G_VM_LOCALSTRINGALLOC, source );
}
