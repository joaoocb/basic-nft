/**
*  @file nft.hpp
*/

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>

using namespace eosio;

//TODO: Issuing, burning and tranfering multiple tokens can be a problem. What happens if one fail?

namespace example {

    CONTRACT nft : public contract {
        public:
            nft(name recvr, name code, datastream<const char*> ds) : contract(recvr, code, ds) {}

            ACTION create(name issuer, symbol sym);

            ACTION remove(symbol sym);

            ACTION issue(name to, symbol sym);

            ACTION burn(name owner, symbol sym, uint64_t tk_id);

            ACTION transfer(name from, name to, symbol sym, uint64_t tk_id, std::string memo);

            TABLE class_parameter {
                uint64_t record_id;
                uint64_t schema_id;
                std::vector<uint8_t> data;

                uint64_t primary_key() const {return record_id;}
            };

            TABLE obj_parameter {
                uint64_t record_id;
                uint64_t schema_id;
                std::vector<uint8_t> data;

                uint64_t primary_key() const {return record_id;}
            };

            TABLE cust_parameter {
                uint64_t record_id;
                uint64_t schema_id;
                std::vector<uint8_t> data;

                uint64_t primary_key() const {return record_id;}
            };

            TABLE schema {
                uint64_t id;
                std::string definition;

                uint64_t primary_key() const {return id;}
            };

            TABLE token {
                uint64_t id;
                uint64_t class_record_id;
                uint64_t obj_record_id;
                uint64_t cust_record_id;
                name owner;

                uint64_t primary_key() const {return id;}
            };

            TABLE token_stat {
                asset data;
                name issuer;

                uint64_t primary_key() const {return data.symbol.code().raw();}
            };

            typedef eosio::multi_index<"classtable"_n, class_parameter> class_table;
            typedef eosio::multi_index<"objtable"_n, obj_parameter> obj_table;
            typedef eosio::multi_index<"custtable"_n, cust_parameter> cust_table;
            typedef eosio::multi_index<"schematable"_n, schema> schema_table;
            typedef eosio::multi_index<"tokentable"_n, token> token_table;
            typedef eosio::multi_index<"tstattable"_n, token_stat> tstat_table;

        private:
            void mint(symbol sym, name issuer, name owner);
   };
} //example namespace
