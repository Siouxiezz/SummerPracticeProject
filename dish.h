#ifndef DISH_H
#define DISH_H

#include "Entity.h"
#include <QString>
#include <QList>

class Dish : public Entity {
public:
    Dish(int id = -1, const QString& name = "", const QString& description = "",
         const QString& category = "", double price = 0.0, double weight = 0.0,
         const QString& imagePath = "");

    int getId() const override { return m_id; }
    QString getName() const { return m_name; }
    QString getDescription() const { return m_description; }
    QString getCategory() const { return m_category; }
    double getPrice() const { return m_price; }
    double getWeight() const { return m_weight; }
    QString getImagePath() const { return m_imagePath; }

    void setId(int id) { m_id = id; }
    void setName(const QString& name) { m_name = name; }
    void setDescription(const QString& description) { m_description = description; }
    void setCategory(const QString& category) { m_category = category; }
    void setPrice(double price) { m_price = price; }
    void setWeight(double weight) { m_weight = weight; }
    void setImagePath(const QString& imagePath) { m_imagePath = imagePath; }

    bool save() override;
    bool load(int id) override;
    bool remove() override;
    QString toString() const override;

    static QList<Dish> getAllDishes();
    static QList<Dish> getDishesByCategory(const QString& category);
    static Dish getById(int id);

    friend bool compareDishesByPrice(const Dish& a, const Dish& b);
    friend bool compareDishesByName(const Dish& a, const Dish& b);

private:
    int m_id;
    QString m_name;
    QString m_description;
    QString m_category;
    double m_price;
    double m_weight;
    QString m_imagePath;
};

#endif // DISH_H