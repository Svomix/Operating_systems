package lab5.CRUD;

import lab5.Entity.Car;
import lombok.Builder;
import lombok.Data;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.Transaction;

import java.util.List;

public class CRUD {
    public static boolean create(SessionFactory sessionFactory, int maxSpeed, int tankCapacity, int enginePower, int price, String name) {
        try (Session session = sessionFactory.openSession()) {
            session.beginTransaction();
            Car newCar = Car.builder().maxSpeed(maxSpeed).enginePower(enginePower).price(price).tankCapacity(tankCapacity).name(name).build();
            session.save(newCar);
            session.getTransaction().commit();
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    public static List<Car> read(SessionFactory sessionFactory, Choice choice, Query query) {
        try (Session session = sessionFactory.openSession()) {
            session.beginTransaction();
            switch (choice) {
                case NAME:
                    return session.createQuery(
                                    "FROM Car c WHERE c.name " + query.getSign() + " :name", Car.class)
                            .setParameter("name", query.getValue())
                            .list();
                case MAX_SPEED:
                    return session.createQuery(
                                    "FROM Car c WHERE c.maxSpeed " + query.getSign() + " :maxSpeed", Car.class)
                            .setParameter("maxSpeed", query.getValue())
                            .list();
                case ENGINE_POWER:
                    return session.createQuery(
                                    "FROM Car c WHERE c.enginePower " + query.getSign() + " :enginePower", Car.class)
                            .setParameter("enginePower", query.getValue())
                            .list();
                case PRICE:
                    return session.createQuery(
                                    "FROM Car c WHERE c.price " + query.getSign() + " :price", Car.class)
                            .setParameter("price", query.getValue())
                            .list();
                case TANK_CAPACITY:
                    return session.createQuery(
                                    "FROM Car c WHERE c.tankCapacity " + query.getSign() + " :tankCapacity", Car.class)
                            .setParameter("tankCapacity", query.getValue())
                            .list();
                default:
                    return null;
            }
        } catch (Exception e) {
            return null;
        }
    }

    public static int update(SessionFactory sessionFactory, Choice fieldToUpdate, Query query, String name) {
        try (Session session = sessionFactory.openSession()) {
            int updatedCount;
            Transaction transaction = null;
            transaction = session.beginTransaction();
            switch (fieldToUpdate) {
                case NAME:
                    updatedCount = session.createQuery("UPDATE Car c SET c.name = :newValue WHERE c.name = :name")
                            .setParameter("newValue", query.getValue())
                            .setParameter("name", name)
                            .executeUpdate();
                    break;
                case MAX_SPEED:
                    updatedCount = session.createQuery("UPDATE Car c SET c.maxSpeed = :newValue WHERE c.name = :name")
                            .setParameter("newValue", Integer.parseInt(query.getValue()))
                            .setParameter("name", name)
                            .executeUpdate();
                    break;
                case ENGINE_POWER:
                    updatedCount = session.createQuery("UPDATE Car c SET c.enginePower = :newValue WHERE c.name = :name")
                            .setParameter("newValue", Integer.parseInt(query.getValue()))
                            .setParameter("name", name)
                            .executeUpdate();
                    break;
                case PRICE:
                    updatedCount = session.createQuery("UPDATE Car c SET c.price = :newValue WHERE c.name = :name")
                            .setParameter("newValue", Double.parseDouble(query.getValue()))
                            .setParameter("name", name)
                            .executeUpdate();
                    break;
                case TANK_CAPACITY:
                    updatedCount = session.createQuery("UPDATE Car c SET c.tankCapacity = :newValue WHERE c.name = :name")
                            .setParameter("newValue", Integer.parseInt(query.getValue()))
                            .setParameter("name", name)
                            .executeUpdate();
                    break;
                default:
                    throw new IllegalArgumentException("Unknown field to update: " + fieldToUpdate);
            }
            transaction.commit();
            return updatedCount;
        } catch (Exception e) {
            e.printStackTrace();
            return 0;
        }
    }

    public static int delete(SessionFactory sessionFactory, Choice choice, Query query) {
        try (Session session = sessionFactory.openSession()) {
            session.beginTransaction();
            int delCount = 0;
            switch (choice) {
                case NAME:
                    delCount = session.createQuery(
                                    "DELETE FROM Car c WHERE c.name " + query.getSign() + " :name")
                            .setParameter("name", query.getValue())
                            .executeUpdate();
                    break;
                case MAX_SPEED:
                    delCount = session.createQuery(
                                    "DELETE FROM Car c WHERE c.maxSpeed " + query.getSign() + " :maxSpeed")
                            .setParameter("maxSpeed", query.getValue())
                            .executeUpdate();
                    break;
                case ENGINE_POWER:
                    delCount = session.createQuery(
                                    "DELETE FROM Car c WHERE c.enginePower " + query.getSign() + " :enginePower")
                            .setParameter("enginePower", query.getValue())
                            .executeUpdate();
                    break;
                case PRICE:
                    delCount = session.createQuery(
                                    "DELETE FROM Car c WHERE c.price " + query.getSign() + " :price")
                            .setParameter("price", query.getValue())
                            .executeUpdate();
                    break;
                case TANK_CAPACITY:
                    delCount = session.createQuery(
                                    "DELETE FROM Car c WHERE c.tankCapacity " + query.getSign() + " :tankCapacity")
                            .setParameter("tankCapacity", query.getValue())
                            .executeUpdate();
                    break;
                default:
                    return delCount;
            }
            session.getTransaction().commit();
            return delCount;
        } catch (Exception e) {
            return 0;
        }
    }

    public enum Choice {
        ENGINE_POWER, MAX_SPEED, PRICE, TANK_CAPACITY, NAME
    }

    @Data
    @Builder
    public static class Query {
        private String value;
        private String sign;
    }
}
