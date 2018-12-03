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

            ACTION create(name issuer, symbol sym, std::string gd_json);

            ACTION remove(symbol sym);

            ACTION issue(name to, symbol sym, std::string td_json, std::vector<std::string> json_data);

            ACTION burn(name owner, symbol sym, std::vector<uint64_t> tk_ids);

            ACTION transfer(name from, name to, symbol sym, std::vector<uint64_t> tk_ids, std::string memo);

            //class parameters
            TABLE generic_data {
                uint128_t hash;
                bool param1;
                bool param2;

                uint64_t primary_key() const {return hash;}
            };
            //object parameters
            TABLE token_data {
                uint128_t hash;
                bool param1;
                bool param2;

                uint64_t primary_key() const {return hash;}
            };
            //token kind parameters
            TABLE token_stat {
                asset data;
                name issuer;
                uint128_t gd_hash; //generic data hash

                uint64_t primary_key() const {return data.symbol.code().raw();}
            };
            //individual token parameters
            TABLE token {
                uint64_t tk_id;
                std::string json_data;
                name owner;
                uint128_t td_hash; //token data hash

                uint64_t primary_key() const {return tk_id;}
            };

            typedef eosio::multi_index<"gdatatable"_n, generic_data> gdata_table;
            typedef eosio::multi_index<"tdatatable"_n, token_data> tdata_table;
            typedef eosio::multi_index<"tstattable"_n, token_stat> tstat_table;
            typedef eosio::multi_index<"tokentable"_n, token> token_table;

        private:
            void mint(symbol sym, std::string json_data, name owner, uint64_t td_hash);
   };
} //example namespace
