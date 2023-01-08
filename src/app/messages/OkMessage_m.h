//
// Generated file, do not edit! Created by opp_msgtool 6.0 from app/messages/OkMessage.msg.
//

#ifndef __TIROCINIO_OKMESSAGE_M_H
#define __TIROCINIO_OKMESSAGE_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// opp_msgtool version check
#define MSGC_VERSION 0x0600
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgtool: 'make clean' should help.
#endif


namespace tirocinio {

class OkMessage;

}  // namespace tirocinio

#include "veins/base/utils/Coord_m.h" // import veins.base.utils.Coord

#include "veins/modules/messages/BaseFrame1609_4_m.h" // import veins.modules.messages.BaseFrame1609_4

#include "veins/base/utils/SimpleAddress_m.h" // import veins.base.utils.SimpleAddress


namespace tirocinio {

/**
 * Class generated from <tt>app/messages/OkMessage.msg:22</tt> by opp_msgtool.
 * <pre>
 * packet OkMessage extends veins::BaseFrame1609_4
 * {
 *     int hostID;
 *     double availableLoad;
 * }
 * </pre>
 */
class OkMessage : public ::veins::BaseFrame1609_4
{
  protected:
    int hostID = 0;
    double availableLoad = 0;

  private:
    void copy(const OkMessage& other);

  protected:
    bool operator==(const OkMessage&) = delete;

  public:
    OkMessage(const char *name=nullptr, short kind=0);
    OkMessage(const OkMessage& other);
    virtual ~OkMessage();
    OkMessage& operator=(const OkMessage& other);
    virtual OkMessage *dup() const override {return new OkMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    virtual int getHostID() const;
    virtual void setHostID(int hostID);

    virtual double getAvailableLoad() const;
    virtual void setAvailableLoad(double availableLoad);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const OkMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, OkMessage& obj) {obj.parsimUnpack(b);}


}  // namespace tirocinio


namespace omnetpp {

template<> inline tirocinio::OkMessage *fromAnyPtr(any_ptr ptr) { return check_and_cast<tirocinio::OkMessage*>(ptr.get<cObject>()); }

}  // namespace omnetpp

#endif // ifndef __TIROCINIO_OKMESSAGE_M_H
