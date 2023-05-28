//
// Generated file, do not edit! Created by opp_msgtool 6.0 from app/messages/AvailabilityMessage.msg.
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
#include "AvailabilityMessage_m.h"

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

namespace task_offloading {

Register_Class(AvailabilityMessage)

AvailabilityMessage::AvailabilityMessage(const char *name, short kind) : ::veins::BaseFrame1609_4(name, kind)
{
}

AvailabilityMessage::AvailabilityMessage(const AvailabilityMessage& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

AvailabilityMessage::~AvailabilityMessage()
{
}

AvailabilityMessage& AvailabilityMessage::operator=(const AvailabilityMessage& other)
{
    if (this == &other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void AvailabilityMessage::copy(const AvailabilityMessage& other)
{
    this->hostID = other.hostID;
    this->index = other.index;
    this->availableLoad = other.availableLoad;
    this->cpuFreq = other.cpuFreq;
    this->senderAddress = other.senderAddress;
    this->vehicleAngle = other.vehicleAngle;
    this->vehicleSpeed = other.vehicleSpeed;
}

void AvailabilityMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->hostID);
    doParsimPacking(b,this->index);
    doParsimPacking(b,this->availableLoad);
    doParsimPacking(b,this->cpuFreq);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->vehicleAngle);
    doParsimPacking(b,this->vehicleSpeed);
}

void AvailabilityMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->hostID);
    doParsimUnpacking(b,this->index);
    doParsimUnpacking(b,this->availableLoad);
    doParsimUnpacking(b,this->cpuFreq);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->vehicleAngle);
    doParsimUnpacking(b,this->vehicleSpeed);
}

int AvailabilityMessage::getHostID() const
{
    return this->hostID;
}

void AvailabilityMessage::setHostID(int hostID)
{
    this->hostID = hostID;
}

const char * AvailabilityMessage::getIndex() const
{
    return this->index.c_str();
}

void AvailabilityMessage::setIndex(const char * index)
{
    this->index = index;
}

double AvailabilityMessage::getAvailableLoad() const
{
    return this->availableLoad;
}

void AvailabilityMessage::setAvailableLoad(double availableLoad)
{
    this->availableLoad = availableLoad;
}

double AvailabilityMessage::getCpuFreq() const
{
    return this->cpuFreq;
}

void AvailabilityMessage::setCpuFreq(double cpuFreq)
{
    this->cpuFreq = cpuFreq;
}

const ::veins::LAddress::L2Type& AvailabilityMessage::getSenderAddress() const
{
    return this->senderAddress;
}

void AvailabilityMessage::setSenderAddress(const ::veins::LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

double AvailabilityMessage::getVehicleAngle() const
{
    return this->vehicleAngle;
}

void AvailabilityMessage::setVehicleAngle(double vehicleAngle)
{
    this->vehicleAngle = vehicleAngle;
}

double AvailabilityMessage::getVehicleSpeed() const
{
    return this->vehicleSpeed;
}

void AvailabilityMessage::setVehicleSpeed(double vehicleSpeed)
{
    this->vehicleSpeed = vehicleSpeed;
}

class AvailabilityMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_hostID,
        FIELD_index,
        FIELD_availableLoad,
        FIELD_cpuFreq,
        FIELD_senderAddress,
        FIELD_vehicleAngle,
        FIELD_vehicleSpeed,
    };
  public:
    AvailabilityMessageDescriptor();
    virtual ~AvailabilityMessageDescriptor();

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

Register_ClassDescriptor(AvailabilityMessageDescriptor)

AvailabilityMessageDescriptor::AvailabilityMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(task_offloading::AvailabilityMessage)), "veins::BaseFrame1609_4")
{
    propertyNames = nullptr;
}

AvailabilityMessageDescriptor::~AvailabilityMessageDescriptor()
{
    delete[] propertyNames;
}

bool AvailabilityMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<AvailabilityMessage *>(obj)!=nullptr;
}

const char **AvailabilityMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *AvailabilityMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int AvailabilityMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 7+base->getFieldCount() : 7;
}

unsigned int AvailabilityMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_hostID
        FD_ISEDITABLE,    // FIELD_index
        FD_ISEDITABLE,    // FIELD_availableLoad
        FD_ISEDITABLE,    // FIELD_cpuFreq
        0,    // FIELD_senderAddress
        FD_ISEDITABLE,    // FIELD_vehicleAngle
        FD_ISEDITABLE,    // FIELD_vehicleSpeed
    };
    return (field >= 0 && field < 7) ? fieldTypeFlags[field] : 0;
}

const char *AvailabilityMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "hostID",
        "index",
        "availableLoad",
        "cpuFreq",
        "senderAddress",
        "vehicleAngle",
        "vehicleSpeed",
    };
    return (field >= 0 && field < 7) ? fieldNames[field] : nullptr;
}

int AvailabilityMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "hostID") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "index") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "availableLoad") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "cpuFreq") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "senderAddress") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "vehicleAngle") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "vehicleSpeed") == 0) return baseIndex + 6;
    return base ? base->findField(fieldName) : -1;
}

const char *AvailabilityMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_hostID
        "string",    // FIELD_index
        "double",    // FIELD_availableLoad
        "double",    // FIELD_cpuFreq
        "veins::LAddress::L2Type",    // FIELD_senderAddress
        "double",    // FIELD_vehicleAngle
        "double",    // FIELD_vehicleSpeed
    };
    return (field >= 0 && field < 7) ? fieldTypeStrings[field] : nullptr;
}

const char **AvailabilityMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *AvailabilityMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int AvailabilityMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void AvailabilityMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'AvailabilityMessage'", field);
    }
}

const char *AvailabilityMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string AvailabilityMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        case FIELD_hostID: return long2string(pp->getHostID());
        case FIELD_index: return oppstring2string(pp->getIndex());
        case FIELD_availableLoad: return double2string(pp->getAvailableLoad());
        case FIELD_cpuFreq: return double2string(pp->getCpuFreq());
        case FIELD_senderAddress: return "";
        case FIELD_vehicleAngle: return double2string(pp->getVehicleAngle());
        case FIELD_vehicleSpeed: return double2string(pp->getVehicleSpeed());
        default: return "";
    }
}

void AvailabilityMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        case FIELD_hostID: pp->setHostID(string2long(value)); break;
        case FIELD_index: pp->setIndex((value)); break;
        case FIELD_availableLoad: pp->setAvailableLoad(string2double(value)); break;
        case FIELD_cpuFreq: pp->setCpuFreq(string2double(value)); break;
        case FIELD_vehicleAngle: pp->setVehicleAngle(string2double(value)); break;
        case FIELD_vehicleSpeed: pp->setVehicleSpeed(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AvailabilityMessage'", field);
    }
}

omnetpp::cValue AvailabilityMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        case FIELD_hostID: return pp->getHostID();
        case FIELD_index: return pp->getIndex();
        case FIELD_availableLoad: return pp->getAvailableLoad();
        case FIELD_cpuFreq: return pp->getCpuFreq();
        case FIELD_senderAddress: return omnetpp::toAnyPtr(&pp->getSenderAddress()); break;
        case FIELD_vehicleAngle: return pp->getVehicleAngle();
        case FIELD_vehicleSpeed: return pp->getVehicleSpeed();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'AvailabilityMessage' as cValue -- field index out of range?", field);
    }
}

void AvailabilityMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        case FIELD_hostID: pp->setHostID(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_index: pp->setIndex(value.stringValue()); break;
        case FIELD_availableLoad: pp->setAvailableLoad(value.doubleValue()); break;
        case FIELD_cpuFreq: pp->setCpuFreq(value.doubleValue()); break;
        case FIELD_vehicleAngle: pp->setVehicleAngle(value.doubleValue()); break;
        case FIELD_vehicleSpeed: pp->setVehicleSpeed(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AvailabilityMessage'", field);
    }
}

const char *AvailabilityMessageDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr AvailabilityMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        case FIELD_senderAddress: return omnetpp::toAnyPtr(&pp->getSenderAddress()); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void AvailabilityMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    AvailabilityMessage *pp = omnetpp::fromAnyPtr<AvailabilityMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'AvailabilityMessage'", field);
    }
}

}  // namespace task_offloading

namespace omnetpp {

}  // namespace omnetpp

