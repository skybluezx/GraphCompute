#ifndef arch_args_h
#define arch_args_h
#include <string>
#include <map>
//#include "nlohmann/json.hpp"

namespace arch {

   // using json = nlohmann::json;
   // using string = std::string;

   // struct payload_error {
   //     int64_t transaction_id;

   //     payload_error(int64_t id)
   //         : transaction_id(id) {};

   //     const char* what() {return "payload error";}
   //     const int64_t get_id() {return transaction_id;}
   // };

    struct In {
        int expected; // expected number
        std::map<std::string, int32_t> questions_assement;
        std::map<std::string, double> current_knowledge_points;
        std::map<std::string, int32_t> preceding_questions_assement;
        std::map<std::string, double> knowledge_points;
    };

    struct Out {
        int32_t code ;
        std::vector<std::string> payload; // recommended question ids
    };

   // struct TransactionIn {
   //     int64_t id;
   //     In payload;
   // };

   // struct TransactionOut {
   //     int64_t id;
   //     Out payload;
   // };

   // void from_json(const nlohmann::json&, In&);
   // void to_json(nlohmann::json&, const Out&);

   // void from_json(const nlohmann::json&, TransactionIn&);
   // void to_json(nlohmann::json&, const TransactionOut&);

}

#endif // end of arch_args_h
