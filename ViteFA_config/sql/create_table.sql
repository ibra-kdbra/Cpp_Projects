
CREATE TABLE IF NOT EXISTS fast_environment (
       env VARCHAR(64) NOT NULL COMMENT 'environment name',
       version BIGINT(20) NOT NULL COMMENT 'current version',
       status TINYINT NOT NULL DEFAULT 0 COMMENT '0: normal, 1: deleted',
       create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT 'create time',
       update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'update time',
       PRIMARY KEY (env),
       UNIQUE (version)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='environment info';

CREATE TABLE IF NOT EXISTS fast_config (
       env VARCHAR(64) NOT NULL COMMENT 'environment name',
       name VARCHAR(64) NOT NULL COMMENT 'config name',
       value TEXT NOT NULL COMMENT 'config value',
       version BIGINT(20) NOT NULL COMMENT 'current version',
       status TINYINT NOT NULL DEFAULT 0 COMMENT '0: normal, 1: deleted',
       type TINYINT NOT NULL DEFAULT 1 COMMENT 'config type, 1: string, 3: list, 5: map',
       create_time DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT 'create time',
       update_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT 'update time',
       PRIMARY KEY (env, name),
       UNIQUE (version)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='config item';

CREATE TABLE IF NOT EXISTS fast_increment (
        name VARCHAR(64) NOT NULL COMMENT 'increment name',
        value BIGINT(20) NOT NULL COMMENT 'current value',
        PRIMARY KEY (name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='for increment';

INSERT INTO fast_increment (name, value) VALUES ('fast_config_version', 0);
INSERT INTO fast_increment (name, value) VALUES ('fast_environment_version', 0);
