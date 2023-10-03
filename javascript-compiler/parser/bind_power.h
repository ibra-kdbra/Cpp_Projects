#pragma once
namespace simple_parser {

enum bp_type {
    BP_NONE = 0,      //No binding ability

   //From top to bottom, the priority becomes higher and higher
//    BP_LOWEST,    //Minimum binding capacity
   BP_ASSIGN = 10,    // =
//    BP_CONDITION,   // ?:
   BP_LOGIC_OR = 30,    // ||
   BP_LOGIC_AND = 35,   // &&
   BP_EQUAL = 40,        // == !=
   BP_CMP = 45,       // < > <= >=
//    BP_BIT_OR,    // |
//    BP_BIT_AND,   // &
//    BP_BIT_SHIFT, // << >>
//    BP_RANGE,       // .. 
   BP_TERM = 50,	  // + -        done
   BP_FACTOR = 60,	  // * / %      done
   BP_UNARY = 70,    // + - ! ~     done
   BP_CALL = 80,     // . () []
   BP_HIGHEST = 200,
};

}