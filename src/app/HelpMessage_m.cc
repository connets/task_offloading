//
// Generated file, do not edit! Created by opp_msgtool 6.0 from app/HelpMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "HelpMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace tirocinio {

Register_Class(HelpMessage)

HelpMessage::HelpMessage(const char *name, short kind) : ::veins::BaseFrame1609_4(name, kind)
{
}

HelpMessage::HelpMessage(const HelpMessage& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

HelpMessage::~HelpMessage()
{
}

HelpMessage& HelpMessage::operator=(const HelpMessage& other)
{
    if (this == &other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void HelpMessage::copy(const HelpMessage& other)
{
    this->msgContent = other.msgContent;
    this->helpReceived = other.helpReceived;
    this->helpOffered = other.helpOffered;
    this->helperHostIndex = other.helperHostIndex;
    this->vehicleLoad = other.vehicleLoad;
    this->ack = other.ack;
    this->senderAddress = other.senderAddress;
    this->receiverAddress = other.receiverAddress;
}

void HelpMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->msgContent);
    doParsimPacking(b,this->helpReceived);
    doParsimPacking(b,this->helpOffered);
    doParsimPacking(b,this->helperHostIndex);
    doParsimPacking(b,this->vehicleLoad);
    doParsimPacking(b,this->ack);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->receiverAddress);
}

void HelpMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->msgContent);
    doParsimUnpacking(b,this->helpReceived);
    doParsimUnpacking(b,this->helpOffered);
    doParsimUnpacking(b,this->helperHostIndex);
    doParsimUnpacking(b,this->vehicleLoad);
    doParsimUnpacking(b,this->ack);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->receiverAddress);
}

const char * HelpMessage::getMsgContent() const
{
    return this->msgContent.c_str();
}

void HelpMessage::setMsgContent(const char * msgContent)
{
    this->msgContent = msgContent;
}

bool HelpMessage::getHelpReceived() const
{
    return this->helpReceived;
}

void HelpMessage::setHelpReceived(bool helpReceived)
{
    this->helpReceived = helpReceived;
}

bool HelpMessage::getHelpOffered() const
{
    return this->helpOffered;
}

void HelpMessage::setHelpOffered(bool helpOffered)
{
    this->helpOffered = helpOffered;
}

int HelpMessage::getHelperHostIndex() const
{
    return this->helperHostIndex;
}

void HelpMessage::setHelperHostIndex(int helperHostIndex)
{
    this->helperHostIndex = helperHostIndex;
}

int HelpMessage::getVehicleLoad() const
{
    return this->vehicleLoad;
}

void HelpMessage::setVehicleLoad(int vehicleLoad)
{
    this->vehicleLoad = vehicleLoad;
}

bool HelpMessage::getAck() const
{
    return this->ack;
}

void HelpMessage::setAck(bool ack)
{
    this->ack = ack;
}

const ::veins::LAddress::L2Type& HelpMessage::getSenderAddress() const
{
    return this->senderAddress;
}

void HelpMessage::setSenderAddress(const ::veins::LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

const ::veins::LAddress::L2Type& HelpMessage::getReceiverAddress() const
{
    return this->receiverAddress;
}

void HelpMessage::setReceiverAddress(const ::veins::LAddress::L2Type& receiverAddress)
{
    this->receiverAddress = receiverAddress;
}

class HelpMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_msgContent,
        FIELD_helpReceived,
        FIELD_helpOffered,
        FIELD_helperHostIndex,
        FIELD_vehicleLoad,
        FIELD_ack,
        FIELD_senderAddress,
        FIELD_receiverAddress,
    };
  public:
    HelpMessageDescriptor();
    virtual ~HelpMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(HelpMessageDescriptor)

HelpMessageDescriptor::HelpMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(tirocinio::HelpMessage)), "veins::BaseFrame1609_4")
{
    propertyNames = nullptr;
}

HelpMessageDescriptor::~HelpMessageDescriptor()
{
    delete[] propertyNames;
}

bool HelpMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<HelpMessage *>(obj)!=nullptr;
}

const char **HelpMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *HelpMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int HelpMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 8+base->getFieldCount() : 8;
}

unsigned int HelpMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_msgContent
        FD_ISEDITABLE,    // FIELD_helpReceived
        FD_ISEDITABLE,    // FIELD_helpOffered
        FD_ISEDITABLE,    // FIELD_helperHostIndex
        FD_ISEDITABLE,    // FIELD_vehicleLoad
        FD_ISEDITABLE,    // FIELD_ack
        0,    // FIELD_senderAddress
        0,    // FIELD_receiverAddress
    };
    return (field >= 0 && field < 8) ? fieldTypeFlags[field] : 0;
}

const char *HelpMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "msgContent",
        "helpReceived",
        "helpOffered",
        "helperHostIndex",
        "vehicleLoad",
        "ack",
        "senderAddress",
        "receiverAddress",
    };
    return (field >= 0 && field < 8) ? fieldNames[field] : nullptr;
}

int HelpMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "msgContent") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "helpReceived") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "helpOffered") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "helperHostIndex") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "vehicleLoad") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "ack") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "senderAddress") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "receiverAddress") == 0) return baseIndex + 7;
    return base ? base->findField(fieldName) : -1;
}

const char *HelpMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",    // FIELD_msgContent
        "bool",    // FIELD_helpReceived
        "bool",    // FIELD_helpOffered
        "int",    // FIELD_helperHostIndex
        "int",    // FIELD_vehicleLoad
        "bool",    // FIELD_ack
        "veins::LAddress::L2Type",    // FIELD_senderAddress
        "veins::LAddress::L2Type",    // FIELD_receiverAddress
    };
    return (field >= 0 && field < 8) ? fieldTypeStrings[field] : nullptr;
}

const char **HelpMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *HelpMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int HelpMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void HelpMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'HelpMessage'", field);
    }
}

const char *HelpMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string HelpMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        case FIELD_msgContent: return oppstring2string(pp->getMsgContent());
        case FIELD_helpReceived: return bool2string(pp->getHelpReceived());
        case FIELD_helpOffered: return bool2string(pp->getHelpOffered());
        case FIELD_helperHostIndex: return long2string(pp->getHelperHostIndex());
        case FIELD_vehicleLoad: return long2string(pp->getVehicleLoad());
        case FIELD_ack: return bool2string(pp->getAck());
        case FIELD_senderAddress: return "";
        case FIELD_receiverAddress: return "";
        default: return "";
    }
}

void HelpMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        case FIELD_msgContent: pp->setMsgContent((value)); break;
        case FIELD_helpReceived: pp->setHelpReceived(string2bool(value)); break;
        case FIELD_helpOffered: pp->setHelpOffered(string2bool(value)); break;
        case FIELD_helperHostIndex: pp->setHelperHostIndex(string2long(value)); break;
        case FIELD_vehicleLoad: pp->setVehicleLoad(string2long(value)); break;
        case FIELD_ack: pp->setAck(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'HelpMessage'", field);
    }
}

omnetpp::cValue HelpMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        case FIELD_msgContent: return pp->getMsgContent();
        case FIELD_helpReceived: return pp->getHelpReceived();
        case FIELD_helpOffered: return pp->getHelpOffered();
        case FIELD_helperHostIndex: return pp->getHelperHostIndex();
        case FIELD_vehicleLoad: return pp->getVehicleLoad();
        case FIELD_ack: return pp->getAck();
        case FIELD_senderAddress: return omnetpp::toAnyPtr(&pp->getSenderAddress()); break;
        case FIELD_receiverAddress: return omnetpp::toAnyPtr(&pp->getReceiverAddress()); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'HelpMessage' as cValue -- field index out of range?", field);
    }
}

void HelpMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        case FIELD_msgContent: pp->setMsgContent(value.stringValue()); break;
        case FIELD_helpReceived: pp->setHelpReceived(value.boolValue()); break;
        case FIELD_helpOffered: pp->setHelpOffered(value.boolValue()); break;
        case FIELD_helperHostIndex: pp->setHelperHostIndex(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_vehicleLoad: pp->setVehicleLoad(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_ack: pp->setAck(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'HelpMessage'", field);
    }
}

const char *HelpMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr HelpMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        case FIELD_senderAddress: return omnetpp::toAnyPtr(&pp->getSenderAddress()); break;
        case FIELD_receiverAddress: return omnetpp::toAnyPtr(&pp->getReceiverAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void HelpMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    HelpMessage *pp = omnetpp::fromAnyPtr<HelpMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'HelpMessage'", field);
    }
}

}  // namespace tirocinio

namespace omnetpp {

}  // namespace omnetpp

