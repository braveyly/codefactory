#include "singleList.h"

int main()
{
   ZwCommand *oneCmd = NULL;
   int deleteCmdSeqIDTmp = 0;
   
   printf( "Prepare for creating one cmd" );
   oneCmd = createZwCmd( 1, 1, 0, NULL );
   printf( "Show this cmd detail info\n" );
   printf( "sequenceID %d, NodeID %d, act %d, status %d\n", oneCmd->sequenceID, oneCmd->nodeID, oneCmd->act, oneCmd->status );

   printf( "Put this cmd into global cmd list\n" );
   putZwCmdToList( &gzwCmdList, oneCmd );
   printf( "Show this global list\n" );
   showZwCmdList( gzwCmdList );

   printf( "Prepare for creating another cmd" );
   oneCmd = createZwCmd( 2, 2, 2, NULL );
   printf( "Show this cmd detail info\n" );
   printf( "sequenceID %d, NodeID %d, act %d, status %d\n", oneCmd->sequenceID, oneCmd->nodeID, oneCmd->act, oneCmd->status );
   deleteCmdSeqIDTmp = oneCmd->sequenceID;

   printf( "Put this antother cmd into global cmd list\n" );
   putZwCmdToList( &gzwCmdList, oneCmd );
   printf( "Show this global list\n" );
   showZwCmdList( gzwCmdList );

   printf( "Prepare for creating third cmd" );
   oneCmd = createZwCmd( 3, 3, 3, NULL );
   printf( "Show this cmd detail info\n" );
   printf( "sequenceID %d, NodeID %d, act %d, status %d\n", oneCmd->sequenceID, oneCmd->nodeID, oneCmd->act, oneCmd->status );

   printf( "Put this third cmd into global cmd list\n" );
   putZwCmdToList( &gzwCmdList, oneCmd );
   printf( "Show this global list\n" );
   showZwCmdList( gzwCmdList );   
   
   printf( "Prepare delete the sequence ID %d cmd\n", deleteCmdSeqIDTmp );
   deleteZwCmdFromListBySeqID( &gzwCmdList, deleteCmdSeqIDTmp );
   printf( "Show this global list\n" );
   showZwCmdList( gzwCmdList );   
   
}

