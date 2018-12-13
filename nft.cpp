/**
*  @file nft.cpp
*/
#include "nft.hpp"

void example::nft::create(name issuer, symbol sym) {
    require_auth(_self);

    eosio_assert(is_account(issuer), "issuer account does not exist");

    eosio_assert(sym.is_valid(), "invalid symbol");
    eosio_assert(sym.precision() == 0, "symbol precision must be 0");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing == tstat_table.end(), "token with symbol already exists");

    // Create new token
    tstat_table.emplace(_self, [&](auto& ts) {
       ts.data.symbol = sym;
       ts.data.amount = 0;
       ts.issuer = issuer;
    });
}

void example::nft::remove(symbol sym) {
    require_auth(_self);

    eosio_assert(sym.is_valid(), "invalid symbol");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol doesn't exist");

    eosio_assert(existing->data.amount == 0, "burn all tokens before removing");

    tstat_table.erase(existing);
}

void example::nft::issue(name to, symbol sym) {
    eosio_assert(is_account(to), "to account does not exist");

    eosio_assert(sym.is_valid(), "invalid symbol name");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol does not exist, create token before issuing");

    print_f("account: %", existing->issuer);
    require_auth(existing->issuer);
    //require_auth(name("example.game"));

    eosio_assert(existing->data.amount < existing->data.max_amount, "maximum amount reached");

    // Increase token supply
    tstat_table.modify(existing, _self, [&](auto& ts) {
       ts.data.amount++;
    });

    mint(sym, existing->issuer, to);
}

void example::nft::burn(name owner, symbol sym, uint64_t tk_id) {
    require_auth(owner);

    tstat_table tstat_table(_self, _self.value);

    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol does not exist, create token before issuing");

    const auto token_stat = *existing;
    require_auth(token_stat.issuer);

    token_table token_table(token_stat.issuer, sym.code().raw());

    auto burn_token = token_table.find(tk_id);
    eosio_assert(burn_token != token_table.end(), "token with id does not exist");

    // Lower token supply
    tstat_table.modify(token_stat, _self, [&](auto& ts) {
        ts.data.amount--;
    });

    token_table.erase(burn_token);
}

void example::nft::transfer(name from, name to, symbol sym, uint64_t tk_id, std::string memo) {
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);

    eosio_assert(is_account(to), "to account does not exist");

    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    tstat_table tstat_table(_self, _self.value);
    auto existing = tstat_table.find(sym.code().raw());
    eosio_assert(existing != tstat_table.end(), "token with symbol doesn't exist");

    token_table token_table(existing->issuer, sym.code().raw());

    auto sender_token = token_table.find(tk_id);
    eosio_assert(sender_token != token_table.end(), "token with specified ID does not exist");

    eosio_assert(sender_token->owner == from, "sender does not own token with specified ID");

    // TODO: get issuer from token_stat
    token_table.modify(sender_token, existing->issuer, [&](auto& token) {
        token.owner = to;
    });

    require_recipient(from);
    require_recipient(to);
}

void example::nft::mint(symbol sym, name issuer, name owner) {
    token_table token_table(_self, sym.code().raw());
    token_table.emplace(_self, [&](auto& token) {
        token.id = token_table.available_primary_key();
        token.owner = owner;
    });
}

EOSIO_DISPATCH(example::nft, (create)(remove)(issue)(burn)(transfer))
