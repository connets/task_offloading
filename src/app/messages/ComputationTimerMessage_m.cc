//
// Generated file, do not edit! Created by opp_msgtool 6.0 from app/messages/ComputationTimerMessage.msg.
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
#include "ComputationTimerMessage_m.h"

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

Register_Class(ComputationTimerMessage)

ComputationTimerMessage::ComputationTimerMessage(const char *name, short kind) : ::veins::BaseFrame1609_4(name, kind)
{
}

ComputationTimerMessage::ComputationTimerMessage(const ComputationTimerMessage& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

ComputationTimerMessage::~ComputationTimerMessage()
{
}

ComputationTimerMessage& ComputationTimerMessage::operator=(const ComputationTimerMessage& other)
{
    if (this == &other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void ComputationTimerMessage::copy(const ComputationTimerMessage& other)
{
    this->simulationTime = other.simulationTime;
    this->indexHost = other.indexHost;
    this->loadHost = other.loadHost;
}

void ComputationTimerMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->simulationTime);
    doParsimPacking(b,this->indexHost);
    doParsimPacking(b,this->loadHost);
}

void ComputationTimerMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->simulationTime);
    doParsimUnpacking(b,this->indexHost);
    doParsimUnpacking(b,this->loadHost);
}

::omnetpp::simtime_t ComputationTimerMessage::getSimulationTime() const
{
    return this->simulationTime;
}

void ComputationTimerMessage::setSimulationTime(::omnetpp::simtime_t simulationTime)
{
    this->simulationTime = simulationTime;
}

int ComputationTimerMessage::getIndexHost() const
{
    return this->indexHost;
}

void ComputationTimerMessage::setIndexHost(int indexHost)
{
    this->indexHost = indexHost;
}

double ComputationTimerMessage::getLoadHost() const
{
    return this->loadHost;
}

void ComputationTimerMessage::setLoadHost(double loadHost)
{
    this->loadHost = loadHost;
}

class ComputationTimerMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_simulationTime,
        FIELD_indexHost,
        FIELD_loadHost,
    };
  public:
    ComputationTimerMessageDescriptor();
    virtual ~ComputationTimerMessageDescriptor();

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

Register_ClassDescriptor(ComputationTimerMessageDescriptor)

ComputationTimerMessageDescriptor::ComputationTimerMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(tirocinio::ComputationTimerMessage)), "veins::BaseFrame1609_4")
{
    propertyNames = nullptr;
}

ComputationTimerMessageDescriptor::~ComputationTimerMessageDescriptor()
{
    delete[] propertyNames;
}

bool ComputationTimerMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<ComputationTimerMessage *>(obj)!=nullptr;
}

const char **ComputationTimerMessageDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *ComputationTimerMessageDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int ComputationTimerMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 3+base->getFieldCount() : 3;
}

unsigned int ComputationTimerMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_simulationTime
        FD_ISEDITABLE,    // FIELD_indexHost
        FD_ISEDITABLE,    // FIELD_loadHost
    };
    return (field >= 0 && field < 3) ? fieldTypeFlags[field] : 0;
}

const char *ComputationTimerMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "simulationTime",
        "indexHost",
        "loadHost",
    };
    return (field >= 0 && field < 3) ? fieldNames[field] : nullptr;
}

int ComputationTimerMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "simulationTime") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "indexHost") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "loadHost") == 0) return baseIndex + 2;
    return base ? base->findField(fieldName) : -1;
}

const char *ComputationTimerMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "omnetpp::simtime_t",    // FIELD_simulationTime
        "int",    // FIELD_indexHost
        "double",    // FIELD_loadHost
    };
    return (field >= 0 && field < 3) ? fieldTypeStrings[field] : nullptr;
}

const char **ComputationTimerMessageDescriptor::getFieldPropertyNames(int field) const
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

const char *ComputationTimerMessageDescriptor::getFieldProperty(int field, const char *propertyName) const
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

int ComputationTimerMessageDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void ComputationTimerMessageDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'ComputationTimerMessage'", field);
    }
}

const char *ComputationTimerMessageDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string ComputationTimerMessageDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_simulationTime: return simtime2string(pp->getSimulationTime());
        case FIELD_indexHost: return long2string(pp->getIndexHost());
        case FIELD_loadHost: return double2string(pp->getLoadHost());
        default: return "";
    }
}

void ComputationTimerMessageDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_simulationTime: pp->setSimulationTime(string2simtime(value)); break;
        case FIELD_indexHost: pp->setIndexHost(string2long(value)); break;
        case FIELD_loadHost: pp->setLoadHost(string2double(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ComputationTimerMessage'", field);
    }
}

omnetpp::cValue ComputationTimerMessageDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_simulationTime: return pp->getSimulationTime().dbl();
        case FIELD_indexHost: return pp->getIndexHost();
        case FIELD_loadHost: return pp->getLoadHost();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'ComputationTimerMessage' as cValue -- field index out of range?", field);
    }
}

void ComputationTimerMessageDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        case FIELD_simulationTime: pp->setSimulationTime(value.doubleValue()); break;
        case FIELD_indexHost: pp->setIndexHost(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_loadHost: pp->setLoadHost(value.doubleValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ComputationTimerMessage'", field);
    }
}

const char *ComputationTimerMessageDescriptor::getFieldStructName(int field) const
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

omnetpp::any_ptr ComputationTimerMessageDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void ComputationTimerMessageDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    ComputationTimerMessage *pp = omnetpp::fromAnyPtr<ComputationTimerMessage>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'ComputationTimerMessage'", field);
    }
}

}  // namespace tirocinio

namespace omnetpp {

}  // namespace omnetpp

