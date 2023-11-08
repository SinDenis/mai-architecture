create table if not exists `User` (
    id bigint not null,
    login varchar(128) not null,
    password varchar(128) not null,
    first_name varchar(128),
    last_name varchar(128),
    email varchar(128),
    primary key(id)
);

create table if not exists `Post`(
    id bigint not null,
    summary varchar(255) not null,
    body varchar(255) not null,
    user_id bigint,
    primary key(id)
);

create table if not exists `Message`(
    id bigint not null,
    text varchar(255) not null,
    user_from bigint not null,
    user_to bigint not null,
    primary key(id)
);

create sequence user_id_sequence start with 1 increment by 1;
create sequence post_id_sequence start with 1 increment by 1;
create sequence message_id_sequence start with 1 increment by 1;
