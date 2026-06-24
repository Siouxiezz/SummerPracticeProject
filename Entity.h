#ifndef ENTITY_H
#define ENTITY_H

#include <QString>

class Entity {
public:
    Entity() = default;
    virtual ~Entity() = default;

    virtual bool save() = 0;
    virtual bool load(int id) = 0;
    virtual bool remove() = 0;
    virtual int getId() const = 0;
    virtual QString toString() const = 0;
};

#endif // ENTITY_H