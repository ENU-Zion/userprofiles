#include <enulib/enu.hpp>

using std::string;

class userprofiles_contract : public enumivo::contract
{
public:
  userprofiles_contract(account_name self) : enumivo::contract(self),
                                             userprofiles(_self, _self) {}

  // A simple store for a user's json.
  void set(const account_name owner, const string json)
  {
    require_auth(owner);

    // Quick check to remind the user the payload must be json.
    enumivo_assert(json[0] == '{', "payload must be json");
    enumivo_assert(json[json.size() - 1] == '}', "payload must be json");

    // If entry exists, update it.
    auto target_itr = userprofiles.find(owner);
    if (target_itr != userprofiles.end())
    {
      userprofiles.modify(target_itr, owner, [&](auto &j) {
        j.owner = owner;
        j.json = json;
      });
    }
    else
    { // Otherwise, create a new entry for them.
      userprofiles.emplace(owner, [&](auto &j) {
        j.owner = owner;
        j.json = json;
      });
    }
  }

  void del(const account_name owner)
  {
    require_auth(owner);
    auto target_itr = userprofiles.find(owner);
    userprofiles.erase(target_itr);
  }

private:
  // @abi table userprofiles i64
  struct userprofiles
  {
    account_name owner;
    string json;

    auto primary_key() const { return owner; }
    ENULIB_SERIALIZE(userprofiles, (owner)(json))
  };
  typedef enumivo::multi_index<N(userprofiles), userprofiles> userprofiles_table;
  userprofiles_table userprofiles;
};

ENUMIVO_ABI(userprofiles_contract, (set)(del))
