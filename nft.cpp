/**
*  @file nft.cpp
*/
#include "nft.hpp"

void example::nft::create(name issuer, symbol sym, std::string gd_json) {
    require_auth(_self);

    eosio_assert(is_account(issuer), "issuer account does not exist");

    eosio_assert(sym.is_valid(), "invalid symbol");
    eosio_assert(sym.precision() == 0, "symbol precision must be 0");

    // TODO: Search for generic_data using gd_json hash
    uint64_t gd_hash = 0;
    // TODO: If not found, create new generic_data using gd_json

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing == tdata_table.end(), "token with symbol already exists");

    // Create new token
    tstat_table.emplace(_self, [&](auto& ts) {
       ts.data.symbol = sym;
       ts.data.amount = 0;
       ts.issuer = issuer;
       ts.gd_hash;
    });
}

void example::nft::remove(symbol sym) {
    require_auth(_self);

    eosio_assert(sym.is_valid(), "invalid symbol");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol doesn't exist");

    const auto token_stat = *existing;
    eosio_assert(token_stat.data.amount == 0, "burn all tokens before removing");

    tstat_table.erase(token_stat);
}

void example::nft::issue(name to, symbol sym, std::string td_json, std::vector<std::string> json_data) {
    eosio_assert(is_account(to), "to account does not exist");

    eosio_assert(sym.is_valid(), "invalid symbol name");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol does not exist, create token before issuing");

    const auto token_stat = *existing;
    require_auth(token_stat.issuer);

    const auto quantity = json_data.sinze();
    const auto current_supply = token_stat.data.amount;
    eosio_assert(token_stat.data.max_amount >= current_supply + quantity, "invalid quantity");

    // Increase token supply
    tstat_table.modify(token_stat, _self, [&](auto& ts) {
       ts.data.amount += quantity;
    });

    // TODO: Search for token_data using td_json hash
    uint64_t td_hash = 0;
    // TODO: If not found, create new token_data using td_json

    for(const auto data : json_data) {
        mint(sym, data, to, td_hash);
    }
}

void example::nft::burn(name owner, symbol sym, std::vector<uint64_t> tk_ids) {
    require_auth(owner);

    tstat_table tstat_table(_self, _self.value);
    token_table token_table(owner, sym.code().raw());

    for(const auto id : tk_ids) {
        auto burn_token = token_table.find(id);
        eosio_assert(burn_token != token_table.end(), "token with id does not exist");

        // Lower token supply
        tstat_table.modify(token_stat, _self, [&](auto& ts) {
            ts.data.amount--;
        });

        token_table.erase(burn_token);
    }
}

void example::nft::transfer(name from, name to, symbol sym, std::vector<uint64_t> tk_ids, std::string memo) {
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);

    eosio_assert(is_account(to), "to account does not exist");

    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol doesn't exist");

    token_table token_table(existing->issuer, sym.code().raw());

    for(const auto id : tk_ids) {
        auto sender_token = token_table.find(id);
        eosio_assert(sender_token != token_table.end(), "token with specified ID does not exist");

        eosio_assert(sender_token->owner == from, "sender does not own token with specified ID");

        // TODO: get issuer from token_stat
        token_table.modify(sender_token, existing->issuer, [&](auto& token) {
            token.owner = to;
        });

        require_recipient(from);
        require_recipient(to);
    }
}

void example::nft::mint(symbol sym, std::string json_data, name owner, uint64_t td_hash) {
    token_table token_table(owner, sym.code().raw());
    token_table.emplace(_self, [&](auto& token) {
        //TODO: Create method to get next id
        token.tk_id = 0;
        token.json_data = json_data;
        token.owner = owner;
        token.td_hash = td_hash;
    });
}

EOSIO_DISPATCH(example::nft, (create)(remove)(issue)(burn)(transfer))
