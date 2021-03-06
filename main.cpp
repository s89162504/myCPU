#include <iostream>
#include "mycpu.h"
#include "cpu.h"
#include "program.h"
#include "pipeline_reg.h"
#include "control_signal.h"
#ifdef DEBUG
#include <assert.h>
#endif

using namespace std;

void unitTestPipelineReg()
{
    IF_ID if_id;
    printf("IF_ID::PC = %u\n", IF_ID::PC);
    if_id.setNew(IF_ID::INSTRUCTION, 843215);
    if_id.setNew(IF_ID::PC, 4);
    printf("get old pc: %u\n", if_id.getPre(IF_ID::PC));
    assert( if_id.getPre(IF_ID::PC) == 0 );
    if_id.update();
    if_id.setNew(IF_ID::PC, 8);
    printf("get old pc: %u\n", if_id.getPre(IF_ID::PC));
    assert( if_id.getPre(IF_ID::PC) == 4 );
    if_id.update();
    if_id.setNew(IF_ID::PC, 12);
    printf("get old pc: %u\n", if_id.getPre(IF_ID::PC));
    assert( if_id.getPre(IF_ID::PC) == 8 );
}

void unitTestControlSignal()
{
  ControlSignal cs;
  cs.setType(InstType::RType);
  printf("cs.signal = %u\n", cs.getSignal());
  assert( cs.getBitSignal(ControlSignal::RegWrite) );
  assert( cs.getBitSignal(ControlSignal::RegDst) );
  assert( cs.getBitSignal(ControlSignal::ALUOp1) );
}

int main(int argsNum, char** args)
{
  //freopen("myoutput.txt", "w", stdout);
  if(DEBUG && 0)
  {
    int num = (-1 - ((1 << 26)-1));
    for(int i=31;i>=0;i--)
      if(num & (1 << i))
        printf("1");
      else
        printf("0");
    printf("\n");
    unitTestPipelineReg();
    unitTestControlSignal();
  }

  const int arrInstrInReg[] = {0, 3, 2, 2, 4, 1, 6, 7, 8};
  const int arrInstrInMem[] = {1, 2, 3, 4, 5};

  const int arrGeneralReg[] = {0, 5, -1, 0, 4, 1, 6, 7, 8};
  const int arrGeneralMem[] = {1, 2, 3, 4, 5};

  const int arrDatahazardReg[] = {0, 1, -2, 3, -2, 1, 6, 7, 8};
  const int arrDatahazardMem[] = {1, 2, 3, 4, 5};

  const int arrLwhazardReg[] = {0, 1, 4, 3, 4, 1, 6, 7, 8};
  const int arrLwhazardMem[] = {1, 2, 3, 4, 5};

  const int arrBranchhazardReg[] = {0, 1, 2, 3, 4, 1, 6, 7, 8};
  const int arrBranchhazardMem[] = {1, 2, 3, 4, 5};

  const int arrBranchbonusReg[] = {0, 1, 1, 3, 4, 1, 6, 7, 8};
  const int arrBranchbonusMem[] = {1, 2, 3, 4, 5};

  const char *inputFileNames[] = {"Branchhazard.txt", "General.txt", "Datahazard.txt", "Lwhazard.txt", "Branchbonus.txt", "InstrIn.txt"};
  const char *outputFileNames[] = {"branchResult.txt", "genResult.txt", "dataResult.txt", "loadResult.txt", "bonusResult.txt", "InstrInOut.txt"};
  const int *const arrRegs[] = {arrBranchhazardReg, arrGeneralReg, arrDatahazardReg, arrLwhazardReg, arrBranchbonusReg, arrInstrInReg};
  const int *const arrMems[] = {arrBranchhazardMem, arrGeneralMem, arrDatahazardMem, arrLwhazardMem, arrBranchhazardMem, arrInstrInMem};

  const int programNum = 5; // ignore test input: InstIn.txt

  // build env
  DataMemory mem;
  Register reg;
  CPU myCPU(&mem, &reg);
  Program *pP;

  for(int i = 0 ; i < programNum ; i++)
  {
    freopen(outputFileNames[i], "w", stdout);
    pP = new Program(inputFileNames[i]);
    myCPU.setProgram(pP);
    myCPU.execute();
    // self test output
    if(!myCPU.checkStatus(arrRegs[i], arrMems[i]))
      fprintf(stderr, "ERROR AT CHECK %s\n", outputFileNames[i]);
    //assert(myCPU.checkStatus(arrRegs[i], arrMems[i]));
    // using assert while develop
    myCPU.reset();
  }

}


