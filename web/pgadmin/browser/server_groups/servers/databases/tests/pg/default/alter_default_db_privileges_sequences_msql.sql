

ALTER DEFAULT PRIVILEGES FOR ROLE postgres
    REVOKE ALL ON TABLES FROM PUBLIC;

ALTER DEFAULT PRIVILEGES FOR ROLE postgres
    REVOKE ALL ON SEQUENCES FROM postgres;
ALTER DEFAULT PRIVILEGES FOR ROLE postgres
GRANT USAGE, SELECT ON SEQUENCES TO PUBLIC;